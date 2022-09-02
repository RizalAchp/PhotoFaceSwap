
#include <ImagePoints.hpp>
#include <opencv2/opencv.hpp>

#include "Calculate.hpp"

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);
    // Read input images
    string filename1 = "ted_cruz.jpg";
    string filename2 = "donald_trump.jpg";

    Mat img1         = imread(filename1);
    Mat img2         = imread(filename2);
    Mat img1Warped   = img2.clone();

    // Read points
    vector<Point2f> points1, points2;
    points1 = readPoints(filename1 + ".txt");
    points2 = readPoints(filename2 + ".txt");

    // convert Mat to float data type
    img1.convertTo(img1, CV_32F);
    img1Warped.convertTo(img1Warped, CV_32F);

    // Find convex hull
    vector<Point2f> hull1;
    vector<Point2f> hull2;
    vector<int> hullIndex;

    convexHull(points2, hullIndex, false, false);

    for (size_t i = 0; i < hullIndex.size(); i++)
    {
        hull1.push_back(points1[static_cast<size_t>(hullIndex[i])]);
        hull2.push_back(points2[static_cast<size_t>(hullIndex[i])]);
    }

    // Find delaunay triangulation for points on the convex hull
    vector<vector<int>> dt;
    Rect rect(0, 0, img1Warped.cols, img1Warped.rows);
    calculateDelaunayTriangles(rect, hull2, dt);

    // Apply affine transformation to Delaunay triangles
    for (size_t i = 0; i < dt.size(); i++)
    {
        vector<Point2f> t1, t2;
        // Get points for img1, img2 corresponding to the triangles
        for (size_t j = 0; j < 3; j++)
        {
            t1.push_back(hull1[static_cast<size_t>(dt[i][j])]);
            t2.push_back(hull2[static_cast<size_t>(dt[i][j])]);
        }

        warpTriangle(img1, img1Warped, t1, t2);
    }

    // Calculate mask
    vector<Point> hull8U;
    for (size_t i = 0; i < hull2.size(); i++)
    {
        Point pt(static_cast<int>(hull2[i].x), static_cast<int>(hull2[i].y));
        hull8U.push_back(pt);
    }

    Mat mask = Mat::zeros(img2.rows, img2.cols, img2.depth());
    fillConvexPoly(mask, &hull8U[0], static_cast<int>(hull8U.size()), Scalar(255, 255, 255));

    // Clone seamlessly.
    Rect r       = boundingRect(hull2);
    Point center = (r.tl() + r.br()) / 2;

    Mat output;
    img1Warped.convertTo(img1Warped, CV_8UC3);
    seamlessClone(img1Warped, img2, mask, center, output, NORMAL_CLONE);

    imshow("Face Swapped", output);
    waitKey(0);
    destroyAllWindows();

    return 1;
}
