#ifndef __VIDEOSTREAM__
#define __VIDEOSTREAM__

#include <cstdio>
#include <string>
#include <mutex>
#include <atomic>

class EventMsg;

class VideoStreamer {

public:
  VideoStreamer(int port);
  virtual ~VideoStreamer();

  std::string GetClientName(void);

  unsigned int Get_Port(void);
  unsigned int Get_VideoWidth(void);
  unsigned int Get_VideoHeight(void);
  unsigned int Get_VideoFramerate(void);
  unsigned int Get_VideoDuration(void);

  void Set_VideoWidthHeight(unsigned int width, unsigned int height);
  void Set_VideoFramerate(unsigned int frame);
  void Set_VideoDuration(unsigned int duration);

  bool isRecording(void);

  int RunStream(void);

private:
  int WaitForClient(void);
  FILE *OpenLocalFile(void);
  FILE *OpenPipe(void);

  std::mutex mtx;
  int listen_fd;
  std::atomic_uint VideoWidth, VideoHeight;
  std::atomic_uint VideoFrame, VideoDuration;
  std::atomic_uint Port;
  std::atomic_bool Recording;
  std::string Client_Name;
  EventMsg *Msg;
};

#endif

