/**
 * File: Drawing.cpp
 * Project: DUtilsCV library
 * Author: Dorian Galvez-Lopez
 * Date: September 23, 2010
 * Description: drawing functions
 * License: see the LICENSE.txt file
 *
 */

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "Drawing.h"

using namespace std;
using namespace DUtilsCV;

// ---------------------------------------------------------------------------

namespace DUtilsCV
{
  #define RANDOM_COLOR cv::Scalar( \
    int(((double)rand()/((double)RAND_MAX + 1.0)) * 256.0), \
    int(((double)rand()/((double)RAND_MAX + 1.0)) * 256.0), \
    int(((double)rand()/((double)RAND_MAX + 1.0)) * 256.0))
}

// ---------------------------------------------------------------------------


void Drawing::drawKeyPoints(cv::Mat &image, 
    const std::vector<cv::KeyPoint> &keypoints,
    bool colorOctave, bool useKeypointSize, bool drawAngle, bool useCartesianAngle)
{
  cv::Scalar colors[4] = {
    cv::Scalar(0, 0, 255),
    cv::Scalar(0, 255, 0),
    cv::Scalar(255, 0, 0),
    cv::Scalar(255, 255, 255)
  };

  const double PI = 3.14159265;

  vector<cv::KeyPoint>::const_iterator it;

  for(it = keypoints.begin(); it != keypoints.end(); ++it)
  {
    float s;  // circle size
    if (useKeypointSize) {
      s = it->size / 2.f;
    } else {
      s = 3.f;
    }
    int r1 = (int)(it->pt.y + 0.5);
    int c1 = (int)(it->pt.x + 0.5);
    cv::Scalar color;
    // if(!colorOctave || it->octave < 1 || it->octave > 3)
    //   color = &colors[3];
    // else
    //   color = &colors[it->octave-1];
    if (colorOctave) {
      if (it->octave < 1 || it->octave > 3) {
        color = colors[3];
      } else {
        color = colors[it->octave-1];
      }
    } else {
      color = RANDOM_COLOR;
    }

    cv::circle(image, cv::Point(c1, r1), (int)s, color, 1);
    
    if (!drawAngle) {
      continue;
    }

    // use keypoint size to draw angle
    s = it->size / 2.f;
    if(it->angle >= 0)
    {
      // angle is in [0..360]
      float o = (float)(it->angle * PI / 180.);
      int c2 = (int)(s * cos(o) + it->pt.x + 0.5);
      int r2;
      
      if(useCartesianAngle)
        r2 = (int)(- s * sin(o) + it->pt.y + 0.5);
      else
        r2 = (int)(s * sin(o) + it->pt.y + 0.5);

      cv::line(image, cv::Point(c1, r1), cv::Point(c2, r2), color);
    }
  }
}

// ---------------------------------------------------------------------------

void Drawing::saveKeyPointImage(const std::string &filename,
    const cv::Mat &image, const std::vector<cv::KeyPoint> &keypoints)
{
  cv::Mat im = image.clone();
  Drawing::drawKeyPoints(im, keypoints);
  cv::imwrite(filename, im);
}

// ---------------------------------------------------------------------------

void Drawing::saveCorrespondenceImage(const std::string &filename, 
    const cv::Mat &im1,
    const cv::Mat &im2, const std::vector<cv::KeyPoint> &kp1,
    const std::vector<cv::KeyPoint> &kp2,
    const std::vector<int> &c1, const std::vector<int> &c2)
{
  cv::Mat image;
  Drawing::drawCorrespondences(image, im1, im2, kp1, kp2, c1, c2);
  cv::imwrite(filename, image);
}

// ---------------------------------------------------------------------------

void Drawing::drawCorrespondences(cv::Mat &image, const cv::Mat &img1,
    const cv::Mat &img2, const std::vector<cv::KeyPoint> &kp1,
    const std::vector<cv::KeyPoint> &kp2,
    const std::vector<int> &c1, const std::vector<int> &c2)
{
  // int rows = img1.rows + img2.rows;
  // int cols = (img1.cols > img2.cols ? img1.cols : img2.cols);
  
  cv::Mat aux1, aux2;
  // if(img1.channels() > 1)
  //   cv::cvtColor(img1, aux1, cv::COLOR_RGB2GRAY);
  // else
  //   aux1 = img1.clone();
  
  // if(img2.channels() > 1)
  //   cv::cvtColor(img2, aux2, cv::COLOR_RGB2GRAY);
  // else
  //   aux2 = img2.clone();

  aux1 = img1.clone();
  aux2 = img2.clone();

  Drawing::drawKeyPoints(aux1, kp1);
  Drawing::drawKeyPoints(aux2, kp2);

  // cv::Mat im = cv::Mat::zeros(rows, cols, CV_8UC1);

  // cv::Rect roi;
  // roi.x = 0;
  // roi.y = 0;
  // roi.width = img1.cols;
  // roi.height = img1.rows;

  // im(roi) = aux1 * 1;
  
  // roi.x = 0;
  // roi.y = img1.rows;
  // roi.width = img2.cols;
  // roi.height = img2.rows;

  // im(roi) = aux2 * 1;
	
  // // draw correspondences
  // cv::cvtColor(im, image, cv::COLOR_GRAY2RGB);

  // concatenate aux1 and aux2 to image
  // clear image

  cv::hconcat(aux1, aux2, image);

  for(unsigned int i = 0; i < c1.size(); ++i)
  {
    int mx = (int)kp1[ c1[i] ].pt.x;
    int my = (int)kp1[ c1[i] ].pt.y;
    int px = (int)kp2[ c2[i] ].pt.x;
    int py = (int)kp2[ c2[i] ].pt.y;

    // py += img1.rows;
    px += img1.cols;

    cv::Scalar color = cv::Scalar( 
      int(((double)rand()/((double)RAND_MAX + 1.0)) * 256.0),
      int(((double)rand()/((double)RAND_MAX + 1.0)) * 256.0),
      int(((double)rand()/((double)RAND_MAX + 1.0)) * 256.0));

    cv::line(image, cv::Point(mx, my), cv::Point(px, py), color, 1);
  }
}

// ---------------------------------------------------------------------------

void Drawing::drawCorrespondences(cv::Mat &image, const cv::Mat &im1, const cv::Mat &im2,
                                  const std::vector<cv::KeyPoint> &kp1,
                                  const std::vector<cv::KeyPoint> &kp2,
                                  const std::vector<int> &matches, bool drawUnmatchedKeypoints)
{
  std::vector<cv::DMatch> matches1to2;
  std::vector<char> matchesMask;
  for (size_t i = 0; i < matches.size(); ++i) {
    matches1to2.push_back(cv::DMatch(i, matches[i], 0));
    if (matches[i] >= 0) {
      matchesMask.push_back(1);
    } else {
      matchesMask.push_back(0);
    }
  }

  cv::DrawMatchesFlags flags = cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS;
  if (drawUnmatchedKeypoints) {
    flags = cv::DrawMatchesFlags::DEFAULT;
  }
  cv::drawMatches(im1, kp1, im2, kp2, matches1to2, image,
                  cv::Scalar::all(-1), cv::Scalar::all(-1), matchesMask, flags);
}

void Drawing::drawReferenceSystem(cv::Mat &image, const cv::Mat &cTo,
    const cv::Mat &A, const cv::Mat &K, float length)
{
  const cv::Mat cRo(cTo, cv::Range(0,3), cv::Range(0,3));
  cv::Mat cto = cTo(cv::Range(0,3), cv::Range(3,4)).clone();
  
  if(cTo.type() == CV_32F)
  {
    cto.at<float>(0,0) /= cTo.at<float>(3,3);
    cto.at<float>(1,0) /= cTo.at<float>(3,3);
    cto.at<float>(2,0) /= cTo.at<float>(3,3);
  }
  else
  {
    cto.at<double>(0,0) /= cTo.at<double>(3,3);
    cto.at<double>(1,0) /= cTo.at<double>(3,3);
    cto.at<double>(2,0) /= cTo.at<double>(3,3);
  }
  
  Drawing::drawReferenceSystem(image, cRo, cto, A, K, length);
}
    
// ---------------------------------------------------------------------------

void Drawing::drawReferenceSystem(cv::Mat &image, const cv::Mat &cRo,
  const cv::Mat &cto, const cv::Mat &A, const cv::Mat &K,
  float length)
{
  cv::Mat k;
  if(K.empty())
    k = cv::Mat::zeros(4,1, cRo.type());
  else
    k = K;

  std::vector<cv::Point3f> oP;
  oP.push_back(cv::Point3f(0,0,0));
  oP.push_back(cv::Point3f(length,0,0));
  oP.push_back(cv::Point3f(0,length,0));
  oP.push_back(cv::Point3f(0,0,length));

  vector<cv::Point2f> points2d;
  cv::projectPoints(cv::Mat(oP), cRo, cto, A, k, points2d);
  
  // draw axis
  cv::Scalar bluez, greeny, redx;
  
  if(image.channels() == 3 )
  {
    bluez = cv::Scalar(255,0,0);
    greeny = cv::Scalar(0,255,0);
    redx = cv::Scalar(0,0,255);
  }
  else
  {
    bluez = cv::Scalar(18,18,18);
    greeny = cv::Scalar(182,182,182);
    redx = cv::Scalar(120,120,120);
  }

  cv::line(image, points2d[0], points2d[1], redx, 2);
  cv::line(image, points2d[0], points2d[2], greeny, 2);
  cv::line(image, points2d[0], points2d[3], bluez, 2);
}

// ---------------------------------------------------------------------------

void Drawing::drawBox(cv::Mat &image, const cv::Mat &cRo,
  const cv::Mat &cto, float width, float height,
  const cv::Mat &A, const cv::Mat &K,
  std::vector<cv::Point2f> *_box,
  const Plot::Style &style) 
{
  vector<cv::Point2f> auxbox;
  vector<cv::Point2f>* pbox = (_box ? _box : &auxbox);
  vector<cv::Point2f>& box = *pbox;

  cv::Mat k;
  if(K.empty())
    k = cv::Mat::zeros(4,1, cRo.type());
  else
    k = K;
  
  const float w = width / 2.f;
  const float h = height / 2.f;
  cv::Mat oBox = (cv::Mat_<float>(4,3) <<
    -w, -h, 0,
     w, -h, 0,
     w,  h, 0,
    -w,  h, 0);
  
  cv::projectPoints(oBox, cRo, cto, A, k, box);

	cv::line(image, box[0], box[1], style.color, style.thickness);
	cv::line(image, box[1], box[2], style.color, style.thickness);
	cv::line(image, box[2], box[3], style.color, style.thickness);
	cv::line(image, box[3], box[0], style.color, style.thickness);
}

// ---------------------------------------------------------------------------

void Drawing::drawBox(cv::Mat &image, const cv::Mat &sHb, int cols, int rows,
  std::vector<cv::Point2f> *_box, const Plot::Style &style)
{
  vector<cv::Point2f> auxbox;
  vector<cv::Point2f>* pbox = (_box ? _box : &auxbox);
  vector<cv::Point2f>& box = *pbox;
  
  box.resize(4);

  cv::Mat P;
  if(sHb.type() == CV_32F)
  {
    P = sHb * (cv::Mat_<float>(3,4) <<
      0, cols, cols,    0,
      0,    0, rows, rows,
      1,    1,    1,    1);
    
    for(short i = 0; i < 4; ++i)
    {
      box[i].x = P.at<float>(0,i) / P.at<float>(2,i);
      box[i].y = P.at<float>(1,i) / P.at<float>(2,i);
    }
  }
  else
  {
    P = sHb * (cv::Mat_<double>(3,4) <<
      0, cols, cols,    0,
      0,    0, rows, rows,
      1,    1,    1,    1);
    
    for(short i = 0; i < 4; ++i)
    {
      box[i].x = P.at<double>(0,i) / P.at<double>(2,i);
      box[i].y = P.at<double>(1,i) / P.at<double>(2,i);
    }
  }

  cv::line(image, box[0], box[1], style.color, style.thickness);
	cv::line(image, box[1], box[2], style.color, style.thickness);
	cv::line(image, box[2], box[3], style.color, style.thickness);
	cv::line(image, box[3], box[0], style.color, style.thickness);

}

// ---------------------------------------------------------------------------

Drawing::Plot::Style::Style(char c, int _thickness)
{
  int r, g, b;
  r = g = b = 0;
  
  thickness = _thickness;
  
  switch(c)
  {
    case 'b': b = 255; break;
    case 'g': g = 255; break;
    case 'r': r = 255; break;
    case 'c': b = g = 255; break;
    case 'm': r = b = 255; break;
    case 'y': r = g = 255; break;
    case 'w': r = g = b = 255; break;
    default: break;
  }
  
  color = cv::Scalar(b, g, r);
}

// ---------------------------------------------------------------------------

Drawing::Plot::Style::Style(int _thickness, char c)
{  
  Drawing::Plot::Style(c, _thickness);
}

// ---------------------------------------------------------------------------

Drawing::Plot::Plot(int rows, int cols, double minx, double maxx, 
  double miny, double maxy, int margin)
{  
  m_bg = cv::Scalar(255,255,255);
  m_margin = margin;
  m_canvas.create(rows, cols, CV_8UC3);
  m_canvas = m_bg;
  setAxes(minx, maxx, miny, maxy, margin);
}

// ---------------------------------------------------------------------------

void Drawing::Plot::create(int rows, int cols, 
  double minx, double maxx, double miny, double maxy, int margin)
{
  m_margin = margin;
  m_canvas.create(rows, cols, CV_8UC3);
  setAxes(minx, maxx, miny, maxy, margin);
  m_canvas = m_bg;
}

// ---------------------------------------------------------------------------

void Drawing::Plot::create(int rows, int cols, 
  double minx, double maxx, double miny, double maxy)
{
  m_canvas.create(rows, cols, CV_8UC3);
  setAxes(minx, maxx, miny, maxy, m_margin);
  m_canvas = m_bg;
}

// ---------------------------------------------------------------------------

void Drawing::Plot::create(double minx, double maxx, double miny, double maxy,
  int margin)
{
  m_margin = margin;
  setAxes(minx, maxx, miny, maxy, m_margin);
  m_canvas = m_bg;
}

// ---------------------------------------------------------------------------

void Drawing::Plot::create(double minx, double maxx, double miny, double maxy)
{
  setAxes(minx, maxx, miny, maxy, m_margin);
  m_canvas = m_bg;
}

// ---------------------------------------------------------------------------

void Drawing::Plot::setAxes(double minx, double maxx, double miny, double maxy, 
  int margin)
{    
  m_cx = (minx + maxx) / 2;
  m_cy = (miny + maxy) / 2;
  double xdif = maxx - minx;
  double ydif = maxy - miny;
  
  m_uppx = xdif / double(m_canvas.cols - margin*2); // units per pixel
  m_uppy = ydif / double(m_canvas.rows - margin*2);
  
  // axis equal
  if(m_uppx > m_uppy)
    m_uppy = m_uppx;
  else
    m_uppx = m_uppy;
}

// ---------------------------------------------------------------------------


void Drawing::Plot::line(double x1, double y1, double x2, double y2, 
  const Style &style)
{
  cv::Point a(toPxX(x1), toPxY(y1));
  cv::Point b(toPxX(x2), toPxY(y2));
    
  cv::line(m_canvas, a, b, style.color, style.thickness);
}

// ---------------------------------------------------------------------------
