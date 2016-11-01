/*
man			: 0 ~ 99, 100?
beach		: 101 ~ 199, 100?
building	: 200 ~ 299
bus			: 300 ~ 399
dinosaur	: 400 ~ 499
elephant	: 500 ~ 599
flower		: 600 ~ 699
horse		: 700 ~ 799
mountain	: 800 ~ 899
food		: 900 ~ 999
*/



#include "ImageScore.h"
using namespace std;

//not support: 5, >7
//string files[] = {"man", "beach", "building", "bus", "dinosaur", "elephant", "flower", "horse", "mountain", "food"};
bool isMatch(int imgid, int targetClass){
	//return targetClass == imgsc.classID();

	switch (targetClass){
	case 0: //man
		if (imgid >= 0 && imgid <= 100)
			return true;
		break;
	case 1: //beach
		if (imgid >= 100 && imgid <= 199)
			return true;
		break;
	case 2: //building
		if (imgid >= 200 && imgid <= 299)
			return true;
		break;
	case 3: //bus
		if (imgid >= 300 && imgid <= 399)
			return true;
		break;
	case 4: //dinosaur
		if (imgid >= 400 && imgid <= 499)
			return true;
		break;
	case 6: //flower
		if (imgid >= 600 && imgid <= 699)
			return true;
		break;
	case 7: //horse
		if (imgid >= 700 && imgid <= 799)
			return true;
		break;
	
	}
	return false;

}

// 𝑃𝑟𝑒𝑐𝑖𝑠𝑖𝑜𝑛 = 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠 𝑡ℎ𝑎𝑡 𝑎𝑟𝑒 𝑓𝑟𝑜𝑚 𝑡ℎ𝑒 𝑐𝑜𝑟𝑟𝑒𝑐𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 / 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑒𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠

// 𝑅𝑒𝑐𝑎𝑙𝑙 = 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠 𝑡ℎ𝑎𝑡 𝑎𝑟𝑒 𝑓𝑟𝑜𝑚 𝑡ℎ𝑒 𝑐𝑜𝑟𝑟𝑒𝑐𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 / 𝑡𝑜𝑡𝑎𝑙 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑖𝑚𝑎𝑔𝑒𝑠 𝑖𝑛 𝑡ℎ𝑒 𝑡𝑎𝑟𝑔𝑒𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 𝑜𝑓 𝑡ℎ𝑒 𝑑𝑎𝑡𝑎𝑠𝑒𝑡

