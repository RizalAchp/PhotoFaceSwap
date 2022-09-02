#include <Calculate.hpp>
#include <ImagePoints.hpp>

namespace cv
{
    // Apply affine transform calculated using srcTri and dstTri to src
    void applyAffineTransform(Mat &warpImage, Mat &src, ImagePoints2f &srcTri,
                              ImagePoints2f &dstTri)
    {
        // Given a pair of triangles, find the affine transform.
        Mat warpMat = getAffineTransform(srcTri, dstTri);

        // Apply the Affine Transform just found to the src image
        warpAffine(src, warpImage, warpMat, warpImage.size(), INTER_LINEAR, BORDER_REFLECT_101);
    }

    // Calculate Delaunay triangles for set of points
    // Returns the vector of indices of 3 points for each triangle
    void calculateDelaunayTriangles(Rect rect, ImagePoints2f &points,
                                    std::vector<std::vector<int>> &delaunayTri)
    {
        // Create an instance of Subdiv2D
        Subdiv2D subdiv(rect);

        // Insert points into subdiv
        for (ImagePoints2f::iterator it = points.begin(); it != points.end(); it++)
            subdiv.insert(*it);

        std::vector<Vec6f> triangleList;
        subdiv.getTriangleList(triangleList);
        ImagePoints2f pt(3);
        std::vector<int> ind(3);

        for (size_t i = 0; i < triangleList.size(); i++)
        {
            Vec6f t = triangleList[i];
            pt[0]   = {t[0], t[1]};
            pt[1]   = {t[2], t[3]};
            pt[2]   = {t[4], t[5]};

            if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
            {
                for (size_t j = 0; j < 3; j++)
                    for (size_t k = 0; k < points.size(); k++)
                        if (abs(pt[j].x - points[k].x) < 1.0 && abs(pt[j].y - points[k].y) < 1)
                            ind[j] = static_cast<int>(k);

                delaunayTri.push_back(ind);
            }
        }
    }

    // Warps and alpha blends triangular regions from img1 and img2 to img
    void warpTriangle(Mat &img1, Mat &img2, ImagePoints2f &t1, ImagePoints2f &t2)
    {
        Rect r1 = boundingRect(t1);
        Rect r2 = boundingRect(t2);

        // Offset points by left top corner of the respective rectangles
        ImagePoints2f t1Rect, t2Rect;
        std::vector<Point> t2RectInt;

        for (size_t i = 0; i < 3; i++)
        {
            t1Rect.push_back(
                {t1[i].x - static_cast<float>(r1.x), t1[i].y - static_cast<float>(r1.y)});
            t2Rect.push_back(
                {t2[i].x - static_cast<float>(r2.x), t2[i].y - static_cast<float>(r2.y)});
            t2RectInt.push_back({(int)t2[i].x - r2.x, (int)t2[i].y - r2.y});
        }

        // Get mask by filling triangle
        Mat mask = Mat::zeros(r2.height, r2.width, CV_32FC3);
        fillConvexPoly(mask, t2RectInt, {1.0, 1.0, 1.0}, 16, 0);

        // Apply warpImage to small rectangular patches
        Mat img1Rect;
        img1(r1).copyTo(img1Rect);

        Mat img2Rect = Mat::zeros(r2.height, r2.width, img1Rect.type());

        applyAffineTransform(img2Rect, img1Rect, t1Rect, t2Rect);

        multiply(img2Rect, mask, img2Rect);
        multiply(img2(r2), Scalar(1.0, 1.0, 1.0) - mask, img2(r2));
        img2(r2) = img2(r2) + img2Rect;
    }
}  // namespace cv
