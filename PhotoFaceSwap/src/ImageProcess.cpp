#include <PhotoFaceSwap.hpp>

namespace cv
{
    // Calculate Delaunay triangles for set of points
    // Returns the vector of indices of 3 points for each triangle
    void CalculateDelaunayTriangles(Rect &rect, const ImagePoints2f &points,
                                    std::vector<std::vector<int>> &delaunayTri)
    {
        EXEC_TIMER("calculating Delaunay Trinangle");
        LOG_DEBUG("Create an instance of Subdiv2D");
        Subdiv2D subdiv(rect);

        LOG_DEBUG("Insert points into subdiv");
        for (auto &it : points)
        {
            subdiv.insert(it);

            std::vector<Vec6f> triangleList;
            LOG_DEBUG("get triangle lists from subdiv2d");
            subdiv.getTriangleList(triangleList);

            std::vector<int> ind(3);
            ImagePoints2f pt(3);
            LOG_DEBUG("check indices in triangle lists points");
            for (auto &triangle : triangleList)
            {
                pt[0] = {triangle[0], triangle[1]};
                pt[1] = {triangle[2], triangle[3]};
                pt[2] = {triangle[4], triangle[5]};

                if (!rect.contains(pt[0]) && !rect.contains(pt[1]) &&
                    !rect.contains(pt[2]))
                    continue;

                for (size_t j = 0; j < pt.size(); j++)
                    for (size_t k = 0; k < points.size(); k++)
                    {
                        if (!(abs(pt[j].x - points[k].x) < 1.0) &&
                            !(abs(pt[j].y - points[k].y) < 1))
                            continue;
                        ind[j] = static_cast<int>(k);
                    }

                delaunayTri.push_back(ind);
            }
        }
    }

    // Warps and alpha blends triangular regions from img1 and img2 to img
    void WarpTriangle(Mat &img1, Mat &img2, ImagePoints2f &t1,
                      ImagePoints2f &t2)
    {
        Rect r1 = boundingRect(t1);
        Rect r2 = boundingRect(t2);

        // Offset points by left top corner of the respective rectangles
        ImagePoints2f t1Rect, t2Rect;
        std::vector<Point> t2RectInt;

        for (size_t i = 0; i < 3; i++)
        {
            t1Rect.push_back({t1[i].x - static_cast<float>(r1.x),
                              t1[i].y - static_cast<float>(r1.y)});
            t2Rect.push_back({t2[i].x - static_cast<float>(r2.x),
                              t2[i].y - static_cast<float>(r2.y)});
            t2RectInt.push_back({(int)t2[i].x - r2.x, (int)t2[i].y - r2.y});
        }

        // Get mask by filling triangle
        Mat mask = Mat::zeros(r2.height, r2.width, CV_32FC3);
        fillConvexPoly(mask, t2RectInt, {1.0, 1.0, 1.0}, 16, 0);

        // Apply warpImage to small rectangular patches
        Mat img1Rect;
        img1(r1).copyTo(img1Rect);

        Mat img2Rect = Mat::zeros(r2.height, r2.width, img1Rect.type());

        warpAffine(img1Rect, img2Rect, getAffineTransform(t1Rect, t2Rect),
                   img2Rect.size(), INTER_LINEAR, BORDER_REFLECT_101);

        multiply(img2Rect, mask, img2Rect);
        multiply(img2(r2), Scalar(1.0, 1.0, 1.0) - mask, img2(r2));
        img2(r2) = img2(r2) + img2Rect;
    }

    Mat CalculateMask(const ImagePoints2f &hull, Mat &tgt_mat)
    {
        // Calculate mask
        std::vector<Point> hull8U;
        for (size_t i = 0; i < hull.size(); i++)
        {
            Point pt(static_cast<int>(hull[i].x), static_cast<int>(hull[i].y));
            hull8U.push_back(pt);
        }

        Mat mask = Mat::zeros(tgt_mat.rows, tgt_mat.cols, tgt_mat.depth());
        fillConvexPoly(mask, &hull8U[0], static_cast<int>(hull8U.size()),
                       Scalar(255, 255, 255));

        return mask;
    }

    void ProcessImage(const fs::path &_src, const fs::path &_target,
                      const cv::ConvexHullPoints &points, cv::Mat &output)
    {
        EXEC_TIMER("processing image source and target");
        Mat src_mat = imread(_src);
        Mat tgt_mat = imread(_target);
        Mat warped  = tgt_mat.clone();
        src_mat.convertTo(src_mat, CV_32F);
        warped.convertTo(warped, CV_32F);

        // Find delaunay triangulation for points on the convex hull
        warped.convertTo(warped, CV_32F);

        std::vector<std::vector<int>> dt;
        Rect rect(0, 0, warped.cols, warped.rows);
        CalculateDelaunayTriangles(rect, points.CvxHull_target, dt);
        for (size_t i = 0; i < dt.size(); i++)
        {
            std::vector<Point2f> t1, t2;
            // Get points for img1, img2 corresponding to the triangles
            for (size_t j = 0; j < 3; j++)
            {
                t1.push_back(points.CvxHull_source[dt[i][j]]);
                t2.push_back(points.CvxHull_target[dt[i][j]]);
            }
            WarpTriangle(src_mat, warped, t1, t2);
        }

        // Calculate mask
        Mat mask = CalculateMask(points.CvxHull_target, tgt_mat);
        // Clone seamlessly.
        Rect r = boundingRect(points.CvxHull_target);
        warped.convertTo(warped, CV_8UC3);
        seamlessClone(warped, tgt_mat, mask, (r.tl() + r.br()) / 2, output,
                      NORMAL_CLONE);
    }

    void ShowResult(cv::Mat &result)
    {
        EXEC_TIMER("open result on gui");
        imshow(format("hasil FaceSwapped"), result);
        waitKey(0);
        destroyAllWindows();
    }

    void SaveResult(const fs::path &file, const cv::Mat &result)
    {
        EXEC_TIMER(
            cv::format("saving result into file: %s", file.c_str()).c_str());
        imwrite(file, result);
    }
}  // namespace cv
