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

//string files[] = { "beach", "building", "bus", "dinosaur", "flower", "horse", "man" };

#include "ImageScore.h"
using namespace std;


bool isMatch(const ImageScore& imgsc, int targetClass){
	//return targetClass == imgsc.classID();

	switch (targetClass){
	case 0: //beach
		if (imgsc.id >= 100 && imgsc.id <= 199)
			return true;
		break;
	case 1: //building
		if (imgsc.id >= 200 && imgsc.id <= 299)
			return true;
		break;
	case 2: //bus
		if (imgsc.id >= 300 && imgsc.id <= 399)
			return true;
		break;
	case 3: //dinosaur
		if (imgsc.id >= 400 && imgsc.id <= 499)
			return true;
		break;
	case 4: //flower
		if (imgsc.id >= 600 && imgsc.id <= 699)
			return true;
		break;
	case 5: //horse
		if (imgsc.id >= 700 && imgsc.id <= 799)
			return true;
		break;
	case 6: //man
		if (imgsc.id >= 0 && imgsc.id <= 100)
			return true;
		break;
	}
	return false;

}

// 𝑃𝑟𝑒𝑐𝑖𝑠𝑖𝑜𝑛 = 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠 𝑡ℎ𝑎𝑡 𝑎𝑟𝑒 𝑓𝑟𝑜𝑚 𝑡ℎ𝑒 𝑐𝑜𝑟𝑟𝑒𝑐𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 / 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑒𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠

// 𝑅𝑒𝑐𝑎𝑙𝑙 = 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠 𝑡ℎ𝑎𝑡 𝑎𝑟𝑒 𝑓𝑟𝑜𝑚 𝑡ℎ𝑒 𝑐𝑜𝑟𝑟𝑒𝑐𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 / 𝑡𝑜𝑡𝑎𝑙 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑖𝑚𝑎𝑔𝑒𝑠 𝑖𝑛 𝑡ℎ𝑒 𝑡𝑎𝑟𝑔𝑒𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 𝑜𝑓 𝑡ℎ𝑒 𝑑𝑎𝑡𝑎𝑠𝑒𝑡

