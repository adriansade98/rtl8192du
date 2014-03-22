/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *
 ******************************************************************************/

#define _OSDEP_SERVICE_C_

#include <autoconf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <recv_osdep.h>
#include <linux/vmalloc.h>
#include <rtw_ioctl_set.h>
/*
* Translate the OS dependent @param error_code to OS independent RTW_STATUS_CODE
* @return: one of RTW_STATUS_CODE
*/
inline int RTW_STATUS_CODE(int error_code) {
	if (error_code >=0)
		return _SUCCESS;

	switch (error_code) {
		default:
			return _FAIL;
	}
}

u32 rtw_atoi(u8* s)
{

	int num=0,flag=0;
	int i;
	for (i=0;i<=strlen(s);i++)
	{
	  if (s[i] >= '0' && s[i] <= '9')
		 num = num * 10 + s[i] -'0';
	  else if (s[0] == '-' && i==0)
		 flag =1;
	  else
		  break;
	 }

	if (flag == 1)
	   num = num * -1;

	 return(num);
}

inline u8* _rtw_vmalloc(u32 sz)
{
	u8	*pbuf;

	pbuf = vmalloc(sz);
	return pbuf;
}

int	_rtw_memcmp(void *dst, void *src, u32 sz)
{
/* under Linux/GNU/GLibc, the return value of memcmp for two same mem. chunk is 0 */
	if (!(memcmp(dst, src, sz)))
		return true;
	else
		return false;
}

/*
For the following list_xxx operations,
caller must guarantee the atomic context.
Otherwise, there will be racing condition.
*/
u32	rtw_is_list_empty(struct list_head *phead)
{
	if (list_empty(phead))
		return true;
	else
		return false;
}

void rtw_list_insert_head(struct list_head *plist, struct list_head *phead)
{
	list_add(plist, phead);
}

void rtw_list_insert_tail(struct list_head *plist, struct list_head *phead)
{
	list_add_tail(plist, phead);
}

/* Caller must check if the list is empty before calling list_del_init */

void _rtw_init_sema(struct  semaphore *sema, int init_val)
{
	sema_init(sema, init_val);
}

void _rtw_free_sema(struct  semaphore *sema)
{
}

void _rtw_up_sema(struct  semaphore *sema)
{
	up(sema);
}

u32 _rtw_down_sema(struct  semaphore *sema)
{
	if (down_interruptible(sema))
		return _FAIL;
	else
		return _SUCCESS;
}

void	_rtw_mutex_init(_mutex *pmutex)
{
	mutex_init(pmutex);
}

void	_rtw_mutex_free(_mutex *pmutex)
{
	mutex_destroy(pmutex);
}

void	_rtw_init_queue(struct __queue *pqueue)
{

	INIT_LIST_HEAD(&(pqueue->queue));

	spin_lock_init(&(pqueue->lock));
}

u32	  _rtw_queue_empty(struct __queue *pqueue)
{
	return (rtw_is_list_empty(&(pqueue->queue)));
}

u32 rtw_end_of_queue_search(struct list_head *head, struct list_head *plist)
{
	if (head == plist)
		return true;
	else
		return false;
}

u32	rtw_get_current_time(void)
{
	return jiffies;
}

inline u32 rtw_systime_to_ms(u32 systime)
{
	return systime * 1000 / HZ;
}

inline u32 rtw_ms_to_systime(u32 ms)
{
	return ms * HZ / 1000;
}

/*  the input parameter start use the same unit as returned by rtw_get_current_time */
inline s32 rtw_get_passing_time_ms(u32 start)
{
	return rtw_systime_to_ms(jiffies-start);
}

inline s32 rtw_get_time_interval_ms(u32 start, u32 end)
{
	return rtw_systime_to_ms(end-start);
}

void rtw_sleep_schedulable(int ms)
{
    u32 delta;

    delta = (ms * HZ)/1000;/* ms) */
    if (delta == 0) {
        delta = 1;/*  1 ms */
    }
    set_current_state(TASK_INTERRUPTIBLE);
    if (schedule_timeout(delta) != 0) {
        return ;
    }
    return;
}

void rtw_msleep_os(int ms)
{
	msleep((unsigned int)ms);
}

void rtw_usleep_os(int us)
{
      if (1 < (us/1000))
                msleep(1);
      else
		msleep((us/1000) + 1);
}

void rtw_mdelay_os(int ms)
{
	mdelay((unsigned long)ms);
}

void rtw_udelay_os(int us)
{
      udelay((unsigned long)us);
}

void rtw_yield_os(void)
{
	yield();
}

#define RTW_SUSPEND_LOCK_NAME "rtw_wifi"

struct net_device *rtw_alloc_etherdev_with_old_priv(int sizeof_priv, void *old_priv)
{
	struct net_device *pnetdev;
	struct rtw_netdev_priv_indicator *pnpi;

	pnetdev = alloc_etherdev_mq(sizeof(struct rtw_netdev_priv_indicator), 4);
	if (!pnetdev)
		goto exit;

	pnpi = netdev_priv(pnetdev);
	pnpi->priv=old_priv;
	pnpi->sizeof_priv=sizeof_priv;

exit:
	return pnetdev;
}

struct net_device *rtw_alloc_etherdev(int sizeof_priv)
{
	struct net_device *pnetdev;
	struct rtw_netdev_priv_indicator *pnpi;

	pnetdev = alloc_etherdev_mq(sizeof(struct rtw_netdev_priv_indicator), 4);
	if (!pnetdev)
		goto exit;

	pnpi = netdev_priv(pnetdev);

	pnpi->priv = vzalloc(sizeof_priv);
	if (!pnpi->priv) {
		free_netdev(pnetdev);
		pnetdev = NULL;
		goto exit;
	}

	pnpi->sizeof_priv=sizeof_priv;
exit:
	return pnetdev;
}

void rtw_free_netdev(struct net_device * netdev)
{
	struct rtw_netdev_priv_indicator *pnpi;

	if (!netdev)
		goto exit;

	pnpi = netdev_priv(netdev);

	if (!pnpi->priv)
		goto exit;

	vfree(pnpi->priv);
	free_netdev(netdev);

exit:
	return;
}

int rtw_change_ifname(struct rtw_adapter *padapter, const char *ifname)
{
	struct net_device *pnetdev;
	struct net_device *cur_pnetdev;
	struct rereg_nd_name_data *rereg_priv;
	int ret;

	if (!padapter)
		goto error;

	cur_pnetdev = padapter->pnetdev;
	rereg_priv = &padapter->rereg_nd_name_priv;

	/* free the old_pnetdev */
	if (rereg_priv->old_pnetdev) {
		free_netdev(rereg_priv->old_pnetdev);
		rereg_priv->old_pnetdev = NULL;
	}

	if (!rtnl_is_locked())
		unregister_netdev(cur_pnetdev);
	else
		unregister_netdevice(cur_pnetdev);

	rereg_priv->old_pnetdev=cur_pnetdev;

	pnetdev = rtw_init_netdev(padapter);
	if (!pnetdev)  {
		ret = -1;
		goto error;
	}

	SET_NETDEV_DEV(pnetdev, dvobj_to_dev(adapter_to_dvobj(padapter)));

	rtw_init_netdev_name(pnetdev, ifname);

	memcpy(pnetdev->dev_addr, padapter->eeprompriv.mac_addr, ETH_ALEN);

	if (!rtnl_is_locked())
		ret = register_netdev(pnetdev);
	else
		ret = register_netdevice(pnetdev);

	if (ret != 0) {
		RT_TRACE(_module_hci_intfs_c_,_drv_err_,("register_netdev() failed\n"));
		goto error;
	}
	return 0;

error:

	return -1;
}

u64 rtw_modular64(u64 x, u64 y)
{
	return do_div(x, y);
}

u64 rtw_division64(u64 x, u64 y)
{
	do_div(x, y);
	return x;
}

void rtw_buf_free(u8 **buf, u32 *buf_len)
{
	if (!buf || !buf_len)
		return;

	if (*buf) {
		*buf_len = 0;
		kfree(*buf);
		*buf = NULL;
	}
}

void rtw_buf_update(u8 **buf, u32 *buf_len, u8 *src, u32 src_len)
{
	u32 ori_len = 0, dup_len = 0;
	u8 *ori = NULL;
	u8 *dup = NULL;

	if (!buf || !buf_len)
		return;

	if (!src || !src_len)
		goto keep_ori;

	/* duplicate src */
	dup = kmalloc(src_len, GFP_ATOMIC);
	if (dup) {
		dup_len = src_len;
		memcpy(dup, src, dup_len);
	}

keep_ori:
	ori = *buf;
	ori_len = *buf_len;

	/* replace buf with dup */
	*buf_len = 0;
	*buf = dup;
	*buf_len = dup_len;

	/* free ori */
	if (ori && ori_len > 0)
		kfree(ori);
}

/**
 * rtw_cbuf_full - test if cbuf is full
 * @cbuf: pointer of struct rtw_cbuf
 *
 * Returns: true if cbuf is full
 */
inline bool rtw_cbuf_full(struct rtw_cbuf *cbuf)
{
	return (cbuf->write == cbuf->read-1)? true : false;
}

/**
 * rtw_cbuf_empty - test if cbuf is empty
 * @cbuf: pointer of struct rtw_cbuf
 *
 * Returns: true if cbuf is empty
 */
inline bool rtw_cbuf_empty(struct rtw_cbuf *cbuf)
{
	return (cbuf->write == cbuf->read)? true : false;
}

/**
 * rtw_cbuf_push - push a pointer into cbuf
 * @cbuf: pointer of struct rtw_cbuf
 * @buf: pointer to push in
 *
 * Lock free operation, be careful of the use scheme
 * Returns: true push success
 */
bool rtw_cbuf_push(struct rtw_cbuf *cbuf, void *buf)
{
	if (rtw_cbuf_full(cbuf))
		return _FAIL;

	cbuf->bufs[cbuf->write] = buf;
	cbuf->write = (cbuf->write+1)%cbuf->size;

	return _SUCCESS;
}

/**
 * rtw_cbuf_pop - pop a pointer from cbuf
 * @cbuf: pointer of struct rtw_cbuf
 *
 * Lock free operation, be careful of the use scheme
 * Returns: pointer popped out
 */
void *rtw_cbuf_pop(struct rtw_cbuf *cbuf)
{
	void *buf;
	if (rtw_cbuf_empty(cbuf))
		return NULL;

	buf = cbuf->bufs[cbuf->read];
	cbuf->read = (cbuf->read+1)%cbuf->size;

	return buf;
}
