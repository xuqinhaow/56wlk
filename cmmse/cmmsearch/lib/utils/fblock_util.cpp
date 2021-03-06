/**
 * @file fblock_util.cpp
 * @author
 * @date 2013.01.06 09:27:53
 * @version $Revision: 1.1 $ 
 * @brief 实现高速顺序io
 *  
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fblock_util.h"

int
fblock_writer::init (fblock_t * hd, int buffer_size)
{
	m_lib = hd;
	m_bufused = 0;
	m_bufsize = (buffer_size / hd->blocksize + 1) * hd->blocksize;
	m_buf = (char *) malloc (m_bufsize * sizeof (char));
	if (!m_buf)
	{
		return -1;
	}
	return 0;
}

int
fblock_writer::write (const void *item, int item_size)
{

	if (item_size != m_lib->blocksize || m_bufused % m_lib->blocksize != 0)
	{
		return -1;
	}
	if (m_bufused + m_lib->blocksize > m_bufsize && m_bufused > 0)
	{

		int ret =
			fblock_multiappend (m_lib, m_bufused / m_lib->blocksize,
								m_buf);
		if ( (m_bufused/m_lib->blocksize) == ret )
			m_bufused = 0;
		else
			return -1;
	}
	if (m_bufused + m_lib->blocksize > m_bufsize)
	{
		if (1 != fblock_multiappend (m_lib, 1, item))
		{
			return -1;
		}
	}
	else
	{
		memcpy (m_buf + m_bufused, (char *) item, m_lib->blocksize);
		m_bufused += m_lib->blocksize;
	}
	return 0;
}

int
fblock_writer::fini ()
{
	int opret = 0;
	if (m_bufused > 0)
	{
		int ret =
			fblock_multiappend (m_lib, m_bufused / m_lib->blocksize,
								m_buf);
		if ( (m_bufused/m_lib->blocksize) == ret)
			m_bufused = 0;
		else
			opret = -1;
	}

	if (m_buf)
	{
		free (m_buf);
		m_buf = NULL;
	}
	return opret;

}

int
fblock_reader::init (fblock_t * hd, int buffer_size)
{
	int opret = 0;

	m_lib = hd;
	m_bufsize = (buffer_size / hd->blocksize + 1) * hd->blocksize;
	m_buf = (char *) malloc (m_bufsize * sizeof (char));
	if (!m_buf)
	{
		return -1;
	}
	m_done = false;
	m_readnum = m_bufsize / m_lib->blocksize;
	m_curblk = 0;
	m_curpos = 0;
	opret =
		fblock_multiread (m_lib, 0 , m_readnum, m_buf);
	if (opret > 0)
	{
		m_readnum = opret;
		m_curblk += opret;
		m_curpos = 0;
	}
	else
	{
		m_done = true;
	}


	return 0;
}

int
fblock_reader::get_item_and_next (void *item, int item_size)
{
	int opret = 0;

	if (m_lib->blocksize != item_size)
	{
		return -1;
	}
	memcpy (item, m_buf + m_lib->blocksize * m_curpos, m_lib->blocksize);
	m_curpos++;

	if (m_curpos >= m_readnum)
	{
		m_readnum = m_bufsize / m_lib->blocksize;
		opret =
			fblock_multiread (m_lib, m_curblk , m_readnum,
							  m_buf);
		if (opret > 0)
		{
			m_readnum = opret;
			m_curblk += opret;
			m_curpos = 0;
		}
		else
		{
			m_done = true;
		}


	}

	return 0;
}

bool
fblock_reader::is_done ()
{
	return m_done;
}

int
fblock_reader::fini ()
{
	if (m_buf)
	{
		free (m_buf);
		m_buf = NULL;
	}
	return 0;
}



int
mfiles_writer::init (mfiles_t * hd, int buffer_size)
{
	m_lib = hd;
	m_bufused = 0;
	m_bufsize = buffer_size ;//+ 1024;
	m_buf = (char *) malloc (m_bufsize * sizeof (char));
	if (!m_buf)
	{
		return -1;
	}
	memset(m_buf,0,m_bufsize);
	return 0;
}

int
mfiles_writer::write (const void *item, int item_size)
{

	if (m_bufused + item_size > m_bufsize && m_bufused > 0)
	{

	//	UB_LOG_FATAL("m_bufused : %d m_busize : %d item_size : %d c : %d",
	//		m_bufused,m_bufsize,item_size,m_buf[m_bufused-1]);
		int ret = mfiles_append (m_lib, m_buf, m_bufused);

		if (m_bufused == ret)
			m_bufused = 0;
		else
			return -1;
	}
	if (0==m_bufused && item_size > m_bufsize)
	{
		int ret = mfiles_append (m_lib, item, item_size);
		if (item_size != ret)
			return -1;
		else
			return ret;
	}
	else
	{
		memcpy (m_buf + m_bufused, (char *) item, item_size);
		m_bufused += item_size;
	}
	return 0;
}

int
mfiles_writer::fini ()
{
	int opret = 0;
	if (m_bufused > 0)
	{
		int ret = mfiles_append (m_lib, m_buf, m_bufused);

		if (m_bufused == ret)
			m_bufused = 0;
		else
			opret = -1;
	}

	if (m_buf)
	{
		free (m_buf);
		m_buf = NULL;
	}
	return opret;

}




int
mfiles_reader::init (mfiles_t * hd, int buffer_size)
{
	int opret = 0;

	m_lib = hd;
	m_bufsize = buffer_size;

	m_buf = (char *) malloc (m_bufsize * sizeof (char));
	if (!m_buf)
	{
		return -1;
	}
	m_done = false;
	m_readsize = m_bufsize;
	m_offset = 0;
	m_curpos = 0;
	opret = mfiles_pread (m_lib, m_buf, m_readsize, m_offset);
	if (opret > 0)
	{
		m_readsize = opret;
		m_offset += opret;
	}
	else
	{
		m_done = true;
	}


	return 0;
}

int
mfiles_reader::get_item_and_next (void *item, int item_size)
{
	int opret = 0;

	if (item_size < 0)
	{
		return -1;
	}

	int readlen = 0;

	while (readlen < item_size)
	{
		if (true == m_done)
			return -1;
		int copy_size = item_size - readlen;

		if (m_readsize - m_curpos < copy_size)
		{
			copy_size = m_readsize - m_curpos;
		}
		memcpy ((char *) item + readlen, m_buf + m_curpos, copy_size);
		m_curpos += copy_size;
		readlen += copy_size;

		if (m_curpos >= m_readsize)
		{
			m_curpos = 0;
			m_readsize = m_bufsize;
			opret = mfiles_pread (m_lib, m_buf, m_readsize, m_offset);
			if (opret > 0)
			{
				m_readsize = opret;
				m_offset += opret;
			}
			else
			{
				m_done = true;
				return 0;
			}
		}

	}

	return 0;
}

bool
mfiles_reader::is_done ()
{
	return m_done;
}

int
mfiles_reader::fini ()
{
	if (m_buf)
	{
		free (m_buf);
		m_buf = NULL;
	}
	return 0;
}




















/* vim: set ts=4 sw=4 sts=4 tw=100 */
