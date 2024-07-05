#include <stdio.h>  //for open printf
#include <fcntl.h>  //for Open O_RDWR
#include <string.h> //for memset
#include <sys/ioctl.h> //for ioctl
#include <linux/videodev2.h> //for v4l2



#define VIDEO_WIDTH 320  //采集图像的宽度
#define VIDEO_HEIGHT 240 //采集图像的高度
//查看 摄像头设备的能力
int get_capability(int fd)
{
    int ret;
    struct v4l2_capability cap;
    memset(&cap,0,sizeof(struct v4l2_capability));
    /*
    struct v4l2_capability{
    _u8 driver[16];驱动名
    _u8 card[32]; 设备名
    _u8 bus_info[32]; 总线信息
    _u32 version; 版本
    _u32 capabilities; 设备支持的操作
    _u32 device_caps;
    _u32 reserved[3];
    }
    */

   ret = ioctl(fd,VIDIOC_QUERYCAP,&cap); //查看设备能力信息
   if(ret < 0)
   {
    printf("VIDIOC_QUERYCAP failed (%d)\n",ret);
    return ret;
   }
   printf("Driver Info: \n Driver Name:%s \n Card Name: %s \n Bus info:%s\n",cap.driver,cap.card,cap.bus_info);
   printf("Device capabilities: \n");
   if(cap.capbilities& V4L2_CAP_VIDEO_CAPTURE)  //支持视频捕获(截取一帧图像保存)
   {
        printf("support video capture \n");
   }
   //V4L2_CAP_VIDEO_OVERLAY支持视频预览(覆盖)，指无需拷贝，直接存放到显卡的内存，需硬件的DMA支持——>实时预览
   //V4L2_CAP_VIDEO_OUTPUT 支持视频输出
   //V4L2_CAP_RDS_CAPTURE 电台识别
   return ret;
}


int get_support_video_format(int fd)
{
    int ret;
    printf("List device support video format: \n");
    stuct v4l2_fmtdesc fmtdesc;
    memset(&fmtdesc, 0, sizeof(fmtdesc));
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while((ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc))==0) //枚举出支持的视频格式
    {
        fmtdesc.index++;
        printf("{pixelformat = '%c%c%c%c',description= '%s'}\n",fmtdesc.pixelformat&0xFF,(fmtdesc.pixelformat >> 8)&0xFF,(fmtdesc.pixelformat>>16)&0xFF
        (fmtdesc.pixelformat>>24)&0xFF, fmtdesc.description);
    }
    return ret;
}

//设置视频格式
int set_video_format(int fd)
{
    int ret=0;
    struct v4l2_format fmt;
    memset(&fmt,0,sizeof(fmt));
    fmt.type                    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width           = VIDEO_WIDTH;
    fmt.fmt.pix.height          = VIDEO_HEIGHT;
    fmt.fmt.pix.pixelformat     = V4L2_PIX_FMT_MJPEG; //注: 如果是支持mjpg的摄像头最好(白色小摄像头)
                                                      //    普通摄像头用 V4L2_PIX_FMT_YUYV它比较麻烦，要用到jpeg库
    fmt.fmt.pix.field           = V4L2_FIELD_INTERLACED;
    ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
    if(ret<0)
    {
        printf("VIDIOC_S_FMT failed(%d)\n",ret);
        return ret;
    }

    //获取视频格式
    ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
    if(ret<0)
    {
        printf("VIDIOC_G_FMT failed(%d)\n",ret);
        return ret;
    }
    //print Stream Format
    printf("Stream Format Informations:\n");
    printf("type: %d\n",fmt.type);
    printf("width: %d\n",fmt.fmt.pix.width);
    printf("height: %d\n",fmt.fmt.pix.height);
    char fmtstr[8];
    memset(fmtstr,0,8);
    memcpy(&fmt.fmt.pix.pixelformat,4);
    printf("pixelformat: %s\n",fmtstr);
    printf("field :%d\n",fmt.fmt.pix.field);
    printf("bytesperline: %d\n",fmt.fmt.pix.bytesperline);
    printf("sizeimage: %d\n", fmt.fmt.pix.bytesperline);
    printf("colorspace: %d\n",fmt.fmt.pix.colorspace);
    printf("priv: %d\n",fmt.fmt.pix.priv);
    printf("raw_data: %s\n",fmt.fmt.raw_data);
}


int main(int argc, char**argv)
{
    int fd = open("/dev/video0",O_RDWR,0);
    if(fd < 0)
    {
        printf("open /dev/video0 failed\n");
        return -1;
    }
    get_capability(fd);
    return 0;
}