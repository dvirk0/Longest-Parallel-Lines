#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
// using namespaces to nullify use of cv::function(); syntax and std::function();
using namespace std;
using namespace cv;

#define LineThreshold 130


#define lengthThreshold 0.95
#define angleThreshold 0.1


#define vector2D vector< pair< int, double > > 
vector2D SortByLength(vector<Vec4i>);
vector2D bubbleSort(vector2D vec);
//void printVector(vector2D);

vector<int> findLongestParallel(vector<Vec4i>);
double findAngle(Vec4i);
int photos(const string&);

int main() {
    photos("Test1.jpg");
    photos("Test2.jpg");
    photos("Test3.jpg");
    photos("Test4.jpg");
    photos("Test5.jpg");
    photos("Test6.jpg");

}
int photos(const string& input)
{
    // Reading image
    Mat img = imread(input);
    // Display original image
    imshow("original Image", img);
    waitKey();

    // Convert to graycsale
    Mat img_gray;
    cvtColor(img, img_gray, COLOR_BGR2GRAY);
    // Blur the image for better edge detection
    Mat img_blur;
    GaussianBlur(img_gray, img_blur, Size(3, 3), 0);
    // Canny edge detection
    Mat edges;
    Canny(img_blur, edges, 100, 200, 3, false);
    // Display canny edge detected image
    imshow("Canny edge detection", edges);
    waitKey();

	Mat lineEdges;
    cvtColor(edges, lineEdges, COLOR_GRAY2BGR);

    // Probabilistic Line Transform
    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(edges, linesP, 1, CV_PI / 180, LineThreshold, 25,10); // runs the actual detection
    // Draw the lines
    
    for (int i = 0; i < linesP.size(); i++)
    {
        Vec4i l = linesP[i];
        line(lineEdges, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(180, 180, 0), 2, LINE_AA); //cyan lines are all detected straight lines
    }
	imshow("Detected Lines (in red) - Standard Hough Line Transform", lineEdges);

    vector<int> longestparallellines;

	waitKey();
    longestparallellines = findLongestParallel(linesP);


    for (int i = 0; i < longestparallellines.size(); i++)
    {
        int index = longestparallellines[i];
        cout << endl << endl << sqrt(pow((linesP[index][0] - linesP[index][2]), 2) + pow((linesP[index][1] - linesP[index][3]), 2)) << endl;
        Vec4i l = linesP[index];
        line(lineEdges, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 5, LINE_AA); //red lines are the found pair of longest parallel lines
    }
    imshow("Final result", lineEdges);
    waitKey();
    cout << longestparallellines.size();
    destroyAllWindows();
    return 0;
}



vector2D SortByLength(vector<Vec4i> lines) {
    vector2D linesByLength;
    vector2D SortedLines;
    for (int i = 0; i < lines.size(); i++)
    {
        pair<int, double> lineWithLength; // creating an array of lines (position in the originl array, line's length)
        lineWithLength.first = i;
        lineWithLength.second = sqrt(pow((lines[i][0]- lines[i][2]),2) + pow((lines[i][1] - lines[i][3]), 2));
        linesByLength.push_back(lineWithLength);

    }
    
    SortedLines = bubbleSort(linesByLength);
    return SortedLines;
}


/*
void printVector(vector2D vec) {
    for (int i = 0; i < vec.size(); i++)
    {
        cout << "  (" << vec[i].first << "," << vec[i].second << ")";
    }
}
*/

vector2D bubbleSort(vector2D vec)
{
    long i, j;
    for (i = 0; i < vec.size() - 1; i++) {
        for (j = 0; j < vec.size() - i - 1; j++) {
            if (vec[j].second < vec[j + 1].second) {

                swap(vec[j], vec[j + 1]);
            }
        }
    }
    return vec;
}

double findAngle(Vec4i line) {
    if (abs(line[0] - line[2]) == 0) {
        return CV_PI/2;
    }
    double angle = atan((double)(abs(line[1] - line[3]) / abs(line[0] - line[2])));
    return angle;
}

vector<int> findLongestParallel(vector<Vec4i> vec) {
    vector2D sortedLines = SortByLength(vec);
    vector<int> results;
    for (int i = 0; i < sortedLines.size() - 1; i++) {
        int index = i+1;
        
        while ((sortedLines[index].second > ((double)lengthThreshold*(sortedLines[i].second))) && (index < sortedLines.size() - 1 )) {
            if (((findAngle(vec[i]) - angleThreshold) <= findAngle(vec[index])) && (findAngle(vec[index]) <= (findAngle(vec[i]) + angleThreshold))) {
                results.push_back(index);
                results.push_back(i);
                return results;
            }
            index++;
        }
    }
    return results;
}