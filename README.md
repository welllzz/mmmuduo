# mmmuduo

mmmuduo网络库是在学习muduo网络库过程中,为了更好的理解和吸收linux多线程编程知识而编写的简单版网络库.主要实现采用one loop per thread + threadPool的编程模型.
主要功能：
•网络库采用Reator模式,基于非阻塞I/O+I/O复用,采用epoll的LT模式.
•实现了多线程下网络连接的建立,消息的到达,消息的发送,连接断开等功能,用户只需要专注于业务逻辑的编写。
•封装了Socket API,设计了应用层Buffer,任务队列,线程池技术,利用eventfd进行线程同步,基于timerfd实现定时器.
•采用Round-Robin的方式分发连接给线程池中的I/O线程,对于计算任务则分配给任务队列完成.
•epoll会监听三种事件:网络I/O,定时器事件,自身线程唤醒事件.
