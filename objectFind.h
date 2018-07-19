#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/ml/ml.hpp>
#include "boost/filesystem.hpp"

using namespace cv;
using namespace std;
using namespace boost::filesystem;

class objectFind
{
public:
	objectFind(int cluster);
	~objectFind(void);
public:
	map<string, Mat> templates, objects, positive_data, negative_data;
	multimap<string, Mat> train_set;
	map<string, Ptr<CvSVM>>svms;
	vector<string> category_names;
	int categories; 
	int clusters;
	path tempfile,train_file,vocab;
	Mat vocabu; 
	bool built;

	Ptr<FeatureDetector> featureDetector;
	Ptr<DescriptorExtractor> descriptorExtractor;
	Ptr<BOWKMeansTrainer> bowtrainer;
	Ptr<BOWImgDescriptorExtractor> bowDescriptorExtractor;
	Ptr<FlannBasedMatcher> descriptorMatcher;
public:
	//bool categorize(Mat& gray);

	void build_vocab();
	void train_classifiers();
	bool make_train_set(); 
	void make_pos_neg(); 
	string remove_extension(string full); 
};

