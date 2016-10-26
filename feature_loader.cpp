#include "feature_loader.h"

/**
Save the surf of each image to ../surf
*/
void save_allDescriptions_YML() {
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	char imagepath[200];
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting Descriptions from input images...\n");

	while (!feof(fp))
	{
		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath)) > 0)
		{
			printf("%s\n", imagepath);
			char tempname[200];
			sprintf_s(tempname, 200, "../%s", imagepath);

			db_img = imread(tempname); // read database image
			if (!db_img.data)
			{
				printf("Cannot find the database image number %d!\n", db_id + 1);
				system("pause");
				throw "Error";
			}

			SurfFeatureDetector detector;
			SurfDescriptorExtractor extractor;
			vector<KeyPoint> keyPoints;
			Mat descriptors;
			detector.detect(db_img, keyPoints);
			extractor.compute(db_img, keyPoints, descriptors);

			FileStorage fs("../surf/" + to_string(db_id) + ".yml", FileStorage::WRITE);
			fs << "keypoints" << keyPoints;
			fs << "descriptors" << descriptors;

			fs.release();

			db_id++;

		}
	}
	fclose(fp);
}

vector<Mat> load_allDescriptions_YML() {
	vector<Mat> allDescriptors;
	FILE* fp;
	char imagepath[200];
	fopen_s(&fp, SURF_LIST_FILE, "r");
	printf("Extracting Descriptions from input images...\n");

	int i = 0;
	while (!feof(fp))
	{
		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath)) > 0)
		{
			char tempname[200];
			sprintf_s(tempname, 200, "../%s", imagepath); // ../%s
			cout << "reading : " << tempname << endl;
			FileStorage storage = FileStorage(tempname, FileStorage::READ);

			if (storage["descriptors"].empty()){
				cout << "descriptors is empty" << endl;
				//waitESC();
				int a;
				cin >> a;
			}
			
			Mat descriptors;
			//Vector<Mat> descriptorsss;
			//storage["descriptors"] >> descriptors;	

			storage["descriptors"] >> descriptors;
			cout << "descriptors readed" << endl;
			//waitESC();

			allDescriptors.push_back(descriptors);
			storage.release();

			i++;
			if (i == 5)
				break;
		}
		if (i == 5)
			break;
	}
	fclose(fp);

	return allDescriptors;
}

Mat load_allDescriptions() {
	Mat allDescriptors;
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	char imagepath[200];
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting Descriptions from input images...\n");


	while (!feof(fp))
	{
		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath)) > 0)
		{
			printf("%s\n", imagepath);
			char tempname[200];
			sprintf_s(tempname, 200, "../%s", imagepath);

			db_img = imread(tempname); // read database image
			if (!db_img.data)
			{
				printf("Cannot find the database image number %d!\n", db_id + 1);
				system("pause");
				throw "Error";
			}

			SurfFeatureDetector detector;
			SurfDescriptorExtractor extractor;
			vector<KeyPoint> keyPoints;
			Mat descriptors;
			detector.detect(db_img, keyPoints);
			extractor.compute(db_img, keyPoints, descriptors);
			allDescriptors.push_back(descriptors);

			db_id++;

		}
	}
	fclose(fp);
	return allDescriptors;
}

Mat cal_descriptor(BOWImgDescriptorExtractor& bowExtractor, const Mat &db_img) {
	SurfFeatureDetector detector;
	vector<KeyPoint> keyPoints;
	Mat descriptors;
	detector.detect(db_img, keyPoints);
	bowExtractor.compute(db_img, keyPoints, descriptors);
	return descriptors;
}

map<int, Mat> load_mlSample(BOWImgDescriptorExtractor& bowExtractor) {
	map<int, Mat> samples;

	int db_id = 0;
	Mat db_img;
	FILE* fp;
	char imagepath[200];
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting mlSample from input images...\n");
	while (!feof(fp))
	{
		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath)) > 0)
		{
			char tempname[200];
			sprintf_s(tempname, 200, "../%s", imagepath);

			db_img = imread(tempname); // read database image

			samples[db_id / 100].push_back(cal_descriptor(bowExtractor, db_img));

			db_id++;
		}
	}
	fclose(fp);
	return samples;
}

vector<Mat> load_features() {
	vector<Mat> features;
	///Read Database
	int db_id = 0;
	Mat db_img;
	FILE* fp;
	char imagepath[200];
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting features from input images...\n");
	while (!feof(fp))
	{
		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath)) > 0)
		{
			printf("%s\n", imagepath);
			char tempname[200];
			sprintf_s(tempname, 200, "../%s", imagepath);

			db_img = imread(tempname); // read database image
			if (!db_img.data)
			{
				printf("Cannot find the database image number %d!\n", db_id + 1);
				system("pause");
				throw "Error";
			}
			Mat hist_base = rgbMat_to_hsvHist(db_img);
			features.push_back(hist_base);
			db_id++;

		}
	}
	fclose(fp);
	return features;
}