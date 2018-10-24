﻿/*
 * MIT License
 *
 * Copyright (c) 2016 xiongziliang <771730766@qq.com>
 *
 * This file is part of ZLMediaKit(https://github.com/xiongziliang/ZLMediaKit).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DEVICE_DEVICE_H_
#define DEVICE_DEVICE_H_

#include <memory>
#include <string>
#include <functional>
#include "Util/util.h"
#include "Player/Player.h"
#include "RtpCodec/RtpMakerAAC.h"
#include "RtpCodec/RtpMakerH264.h"
#include "Rtsp/RtspToRtmpMediaSource.h"
#include "Rtsp/RtspSdp.h"
#include "Util/TimeTicker.h"

using namespace std;
using namespace toolkit;

#ifdef ENABLE_FAAC
#include "Codec/AACEncoder.h"
#endif //ENABLE_FAAC

#ifdef ENABLE_X264
#include "Codec/H264Encoder.h"
#endif //ENABLE_X264


namespace mediakit {

class VideoInfo {
public:
	int iWidth;
	int iHeight;
	float iFrameRate;
};
class AudioInfo {
public:
	int iChannel;
	int iSampleBit;
	int iSampleRate;
	int iProfile;
};

class DevChannel  : public RtspToRtmpMediaSource{
public:
	typedef std::shared_ptr<DevChannel> Ptr;
    //fDuration<=0为直播，否则为点播
	DevChannel(const char *strVhost,
               const char *strApp,
               const char *strId,
               float fDuration = 0,
               bool bEanbleHls = true,
               bool bEnableMp4 = false);
	virtual ~DevChannel();

	void initVideo(const VideoInfo &info);
	void initAudio(const AudioInfo &info);

	void inputH264(const char *pcData, int iDataLen, uint32_t uiStamp);
	void inputAAC(const char *pcDataWithAdts, int iDataLen, uint32_t uiStamp, bool withAdtsHeader = true);
	void inputAAC(const char *pcDataWithoutAdts,int iDataLen, uint32_t uiStamp,const char *pcAdtsHeader);

#ifdef ENABLE_X264
        void inputYUV(char *apcYuv[3], int aiYuvLen[3], uint32_t uiStamp);
#endif //ENABLE_X264

#ifdef ENABLE_FAAC
        void inputPCM(char *pcData, int iDataLen, uint32_t uiStamp);
#endif //ENABLE_FAAC

private:
	inline void makeSDP_264(unsigned char *pucData, int iDataLen);
	inline void makeSDP_AAC(unsigned char *pucData);
	inline void makeSDP(const string& strSdp);
#ifdef ENABLE_X264
	std::shared_ptr<H264Encoder> _pH264Enc;
#endif //ENABLE_X264

#ifdef ENABLE_FAAC
	std::shared_ptr<AACEncoder> _pAacEnc;
#endif //ENABLE_FAAC
	RtpMaker_AAC::Ptr _pRtpMaker_aac;
	RtpMaker_H264::Ptr _pRtpMaker_h264;
	bool _bSdp_gotH264 = false;
	bool _bSdp_gotAAC = false;

	unsigned char _aucSPS[256];
	unsigned int _uiSPSLen = 0;
	unsigned char _aucPPS[256];
	unsigned int _uiPPSLen = 0;
	std::shared_ptr<VideoInfo> _video;
	std::shared_ptr<AudioInfo> _audio;
    SmoothTicker _aTicker[2];
	std::shared_ptr<AACFrame> _pAdtsHeader;
};


} /* namespace mediakit */

#endif /* DEVICE_DEVICE_H_ */
