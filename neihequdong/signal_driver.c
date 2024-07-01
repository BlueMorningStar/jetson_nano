/******信号通知驱动*********/




struct fasync_struct *async_queue; //定义异步通知

write_dev() //write函数里面加
{
	kill_fasync(&async_queue, SIGIO, POLL_IN);  //发送SIGIO 异步通知信号
}

static int init_fasync(int fd, struct file *filp, int mode)
{
	return fasync_helper(fd, filp, mode, &async_queue);  //初始化异步通知
}

static struct file_operations dev_fops = {
	.fasync = init_fasync,
};