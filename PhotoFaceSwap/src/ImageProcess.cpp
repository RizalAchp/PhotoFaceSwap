#include <PhotoFaceSwap.hpp>

namespace cv
{
    // Calculate Delaunay triangles for set of points
    // Returns the vector of indices of 3 points for each triangle
    void CalculateDelaunayTriangles(
        const Rect &&rect, const ImagePoints2f &points,
        std::vector<std::array<int, 3>> &delaunayTri)
    {
        EXEC_TIMER("calculating Delaunay Trinangle");
        LOG_DEBUG("Create an instance of Subdiv2D");
        Subdiv2D subdiv(rect);

        LOG_DEBUG("Insert points into subdiv");
        delaunayTri.reserve(points.size());
        for (const cv::Point2f &it : points)
        {
            subdiv.insert(it);

            std::vector<Vec6f> triangleList;
            subdiv.getTriangleList(triangleList);

            std::array<int, 3> ind;
            ImagePoints2f pt(3);
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
                        ind[j] = (int)k;
                    }

                delaunayTri.push_back(ind);
            }
        }
    }

    // Warps and alpha blends triangular regions from img1 and img2 to img
    void WarpTriangle(const Mat &img1, const Mat &img2, const ImagePoints2f &t1,
                      const ImagePoints2f &t2)
    {
        Rect r1 = boundingRect(t1);
        Rect r2 = boundingRect(t2);

        // Offset points by left top corner of the respective rectangles
        ImagePoints2f t1Rect(3), t2Rect(3);
        std::vector<Point> t2RectInt(3);

        for (size_t i = 0; i < 3; i++)
        {
            t1Rect[i]    = {t1[i].x - r1.x, t1[i].y - r1.y};
            t2Rect[i]    = {t2[i].x - r2.x, t2[i].y - r2.y};
            t2RectInt[i] = {(int)t2[i].x - r2.x, (int)t2[i].y - r2.y};
        }

        // Get mask by filling triangle
        Mat mask = Mat::zeros(r2.height, r2.width, CV_32FC3);
        fillConvexPoly(mask, t2RectInt, {1.0, 1.0, 1.0}, 16, 0);

        // Apply warpImage to small rectangular patches
        Mat img1Rect;
        img1(r1).copyTo(img1Rect);

        Mat img2Rect = Mat::zeros(r2.height, r2.width, img1Rect.type());

        warpAffine(img1Rect, img2Rect, getAffineTransform(t1Rect, t2Rect),
                   img2Rect.size(), INTER_NEAREST, BORDER_REFLECT_101);

        multiply(img2Rect, mask, img2Rect);
        multiply(img2(r2), Scalar(1.0, 1.0, 1.0) - mask, img2(r2));
        img2(r2) = img2(r2) + img2Rect;
    }

    void CalculateMask(const ImagePoints2f &hull, const Mat &tgt_mat,
                       Mat &mask_out)
    {
        EXEC_TIMER("calculating Mask from hull and mat target");
        // Calculate mask
        mask_out = Mat::zeros(tgt_mat.rows, tgt_mat.cols, tgt_mat.depth());

        const size_t size_hull = hull.size();
        std::vector<Point> hull8U;
        hull8U.reserve(size_hull);
        for (size_t i = 0; i < size_hull; i++)
            hull8U.push_back(Point(hull[i].x, hull[i].y));

        fillConvexPoly(mask_out, &hull8U[0], size_hull, Scalar(255, 255, 255));
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

        std::vector<std::array<int, 3>> dt;
        CalculateDelaunayTriangles({0, 0, warped.cols, warped.rows},
                                   points.CvxHull_target, dt);
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
        Mat mask;
        CalculateMask(points.CvxHull_target, tgt_mat, mask);
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
