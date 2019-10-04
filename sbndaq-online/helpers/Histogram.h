#ifndef _sbndaq_online_Histogram_h_
#define _sbndaq_online_Histogram_h_

#include <string>

class TH1;
class TH2;

namespace sbndaq {
  void SendHistogram(const std::string &key, TH1 *hist, size_t padding_x, size_t padding_y, size_t size_x, size_t size_y);
  void SendHistogram(const std::string &key, TH2 *hist, size_t padding_x, size_t padding_y, size_t size_x, size_t size_y);
}

#endif
