#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CJY61.h"
#include <iostream>
#include <iostream>
#include <chrono>

using namespace std;

CJY61 ::CJY61()
{
}

 unsigned char tmpAcc[8];
 unsigned char tmpGyr[8];
 unsigned char tmpAngle[8];

// 参考:https://wit-motion.yuque.com/wumwnr/bf4d0f/wegquy?#%20%E3%80%8AWT61%E5%8D%8F%E8%AE%AE%E3%80%8B
// head(1) + 数据位(8) + 校验和(2 byte) 
void CJY61::FetchData(char *data, int usLength)
{
    long long theTime = std::chrono::system_clock::now().time_since_epoch().count();
    int frameState = 0;
    int bytenum = 0;
    int checkSum;
    int i = 0;
    while (i < usLength) {
        unsigned char current = data[i];
        if (frameState == 0)  {
            // 防止0x55是data数据中的字符，但是误认为是数据标记起始位
            if (current == 0x55 && bytenum == 0)
            {
                checkSum = current;
                bytenum = 1;
            } else if (current == 0x51 && bytenum == 1) {
                checkSum += current;
                frameState = 1;
                bytenum = 2;
            } else if(current == 0x52 && bytenum == 1) {
                checkSum += current;
                frameState = 2;
                bytenum = 2;
            } else if (current == 0x53 && bytenum == 1) {
                checkSum += current;
                frameState = 3;
                bytenum = 2;
            }
        } else if (frameState == 1) { // acc
            if (bytenum < 10) {
                tmpAcc[bytenum - 2] = data[i];
                checkSum += data[i];
                bytenum++;
            } else {
                // == 优先级大于&
                if (data[i] == (checkSum & 0xff)) {
                    memcpy(&stcAcc, &tmpAcc, 8);
                    acc.x = stcAcc.a[0] / 32768.00 * 16 * 9.8;
                    acc.y = stcAcc.a[1] / 32768.00 * 16 * 9.8;
                    acc.z = stcAcc.a[2] / 32768.00 * 16 * 9.8;
                    cout << "time: " << theTime << " x: " << acc.x << " y: " << acc.y << " z: " << acc.z << endl;
                }
                checkSum = 0;
                bytenum = 0;
                frameState = 0;
            }
        } else if (frameState == 2) { // 角速度 gyr
            if (bytenum < 10) {
                tmpGyr[bytenum - 2] = data[i];
                checkSum += data[i];
                bytenum++;
            } else {
                if (data[i] == (checkSum & 0xff)) {
                    memcpy(&stcGyro, &tmpGyr, 8);
                    gyro.x = stcGyro.w[0] / 32768.00 * 2000;
                    gyro.y = stcGyro.w[1] / 32768.00 * 2000;
                    gyro.z = stcGyro.w[2] / 32768.00 * 2000;
                    //cout << "time: " << theTime << " x: " << gyro.x << " y: " << gyro.y << " z: " << gyro.z << endl;
                }
                checkSum = 0;
                bytenum = 0;
                frameState = 0;
            }
        } else if (frameState == 3) { // angle
           if (bytenum < 10) {
                tmpAngle[bytenum - 2] = data[i];
                checkSum += data[i];
                bytenum++;
            } else {
                if (data[i] == (checkSum & 0xff)) {
                    memcpy(&stcAngle, &tmpAngle, 8);         
                    angle.r = stcAngle.Angle[0] / 32768.00 * 180;
                    angle.p = stcAngle.Angle[1] / 32768.00 * 180;
                    angle.y = stcAngle.Angle[2] / 32768.00 * 180;
                    //cout << "time: " << theTime << "r: " << angle.r << " p: " << angle.p << " y: " << angle.y << endl;
                }
                checkSum = 0;
                bytenum = 0;
                frameState = 0;
            }
        }
        ++i;
    }
}

	// 	switch (pData_head[1])
	// 	{
	// 	case 0x50:
	// 		memcpy(&stcTime, &pData_head[2], 8);
	// 		break;
	// 	case 0x51:   
	// 		memcpy(&stcAcc, &pData_head[2], 8);
	// 		acc.x = stcAcc.a[0] / 32768.00 * 16 * 9.8;
	// 		acc.y = stcAcc.a[1] / 32768.00 * 16 * 9.8;
	// 		acc.z = stcAcc.a[2] / 32768.00 * 16 * 9.8;
	// 		break;
	// 	case 0x52: // 角速度，单位为: 角度/s
	// 		memcpy(&stcGyro, &pData_head[2], 8);
	// 		gyro.x = stcGyro.w[0] / 32768.00 * 2000;
	// 		gyro.y = stcGyro.w[1] / 32768.00 * 2000;
	// 		gyro.z = stcGyro.w[2] / 32768.00 * 2000;
	// 		break;
	// 	case 0x53: // 角度
	// 		memcpy(&stcAngle, &pData_head[2], 8);
	// 		angle.r = stcAngle.Angle[0] / 32768.00 * 180;
	// 		angle.p = stcAngle.Angle[1] / 32768.00 * 180;
	// 		angle.y = stcAngle.Angle[2] / 32768.00 * 180;
	// 		break;
	// 	}
	// 	usLength -= 11;
	// 	pData_head += 11;
	// }
