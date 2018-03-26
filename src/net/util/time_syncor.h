// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_TIME_SYNCOR_H_
#define NET_TIME_SYNCOR_H_

#include <string>
#include "base/basictypes.h"

namespace net {

/*
	TimeSyncor time_syncor;
	��
	if (TimeSyncor:: kMaxTimSamples ==
	  time_syncor.OnRecvTimeSample(sent_time, current_time)) {
	  ����ͬ����ʱ��= ��ǰʱ��+time_syncor.GetCorrectedDelta();
	}
 */
// ʱ��ͬ����
class TimeSyncor {
public:
	static const size_t kMaxTimSamples = 10;

	TimeSyncor() {
		first_delta_flag_ = false;
	}

	// sent_time: �ͻ��˷��͸���������ʱ��
	// current_time: ���������ظ��ͻ��˵�ʱ��
	// return: ����ֵΪ��ѭ���˶��ٴ�
	size_t OnRecvTimeSample(uint32_t sent_time, uint32_t current_time) {
		
		int delta = (current_time-sent_time) / 2;
		if (!first_delta_flag_) {
			first_delta_flag_ = true;
			// ����ϵͳʱ����¼����
			// delta_time_
			return 0;
		} else {
			deltas_.push_back(delta);
			return deltas_.size();
		}
	}

	// ���������ƽ��ֵ
	uint32_t GetCorrectedDelta() {
		DCHECK(deltas_.size() > 2);

		// ����
		std::sort(deltas_.begin(), deltas_.end());

		// ȡ��ֵ
		int median = deltas_[deltas_.size()/2];

		int sum = 0;
		int count = 0;
		for (size_t i=0; i<deltas_.size(); ++i) {
			// ����������ֵ1.5������ֵ
			if (deltas_[i] < 1.5*median) {
				sum += deltas_[i];
				count++;
			}
		}

		// ʹ������ƽ���õ�һ��ƽ��ֵ
		return sum / count;
	}


private:
	// ��׼ʱ��
	uint32_t delta_time_;
	// ����ʱ��������˵�һ����
	std::vector<int> deltas_;
	// ���յ���һ��ʱ������
	bool first_delta_flag_;
};

}

#endif
