/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "cvd/internal/io/jpeg.h"

#include "cvd/image_io.h"
using namespace std;
#include <iostream>
#include <iomanip>
#include <setjmp.h>

namespace CVD
{
namespace JPEG
{

struct jpeg_istream_src: public jpeg_source_mgr
{

	istream* i;
	bool eof;
	static const int bufsize=8192;
	JOCTET buf[bufsize+2];

	//Constructor
	static void create(j_decompress_ptr p, istream* is)
	{
		//Allocate memory
		p->src = (jpeg_istream_src*) (*p->mem->alloc_small)((jpeg_common_struct*)p, JPOOL_PERMANENT, sizeof(jpeg_istream_src));

		jpeg_istream_src* me = (jpeg_istream_src*)p->src;

		//Set up virtual member functions
		me->init_source = s_init_src;
		me->fill_input_buffer = s_fill_input_buffer;
		me->skip_input_data = s_skip_input_data;
		me->resync_to_restart = jpeg_resync_to_restart;
		me->term_source = s_term_source;

		//Set up data members
		me->i = is;
		me->bytes_in_buffer = 0;
		me->next_input_byte = me->buf;
		me->eof = 0;
	}
	

	static void s_init_src(j_decompress_ptr p)
	{
		jpeg_istream_src* me = (jpeg_istream_src*)p->src;

		me->bytes_in_buffer = 0;	
	}

	static boolean s_fill_input_buffer(j_decompress_ptr p)
	{

		int n=0;
		jpeg_istream_src* me = (jpeg_istream_src*)p->src;
		me->next_input_byte = me->buf;

		if(me->eof)
		{
			//Output JPEG end-of-image flags
			me->buf[0] = 0xff;
			me->buf[1] = JPEG_EOI;
			me->bytes_in_buffer = 2;
			return true;
		}

		int c;
		for(n=0; n < bufsize;  n++)
		{
			//Get a byte...
			c = me->i->get();
			

			//Check for EOF...
			if(c  == EOF)
			{
				me->eof = 1;
				break;
			}
			
			//Store the byte...
			me->buf[n] = c;
			
			//ooooh! a marker!
			if(c == 0xff)
			{
				c = me->i->get();
				if(c == EOF)
				{
					me->eof = 1;
					break;
				}


				me->buf[++n] = c;

				if(c == JPEG_EOI)
				{
					me->eof = 1;
					break;
				}
			}
		}

		//me->i->read((char*)(me->buf), bufsize);
		//me->bytes_in_buffer = me->i->gcount();	
		me->bytes_in_buffer = n;

		if(me->i->eof())
			me->eof = 1;

		if(me->bytes_in_buffer == 0)
			s_fill_input_buffer(p);

		return true;
	}

	static void s_skip_input_data(j_decompress_ptr p, long num)
	{
		jpeg_istream_src* me = (jpeg_istream_src*)p->src;


		if(num > (long)(me->bytes_in_buffer))
		{
			me->i->ignore(num - me->bytes_in_buffer);
			me->bytes_in_buffer = 0;

		}

		else
		{
			me->next_input_byte += num;
			me->bytes_in_buffer -= num;
		}
	}

	static void s_term_source(j_decompress_ptr p)
	{
	}
};


void jumpy_error_exit (j_common_ptr cinfo)
{
	//Exceptions don't work from C code, so use setjmp/longjmp to jump back
	//in to C++ code.
	longjmp(*(jmp_buf*)(cinfo->client_data), 1);
}

struct jpeg_error_mgr * jumpy_error_manager (struct jpeg_error_mgr * err)
{
	//Set up most of the useful defaults
	jpeg_std_error(err);
	//Except we don't want to exit on an error.
	err->error_exit = jumpy_error_exit;
	return err;
}

jpeg_in::jpeg_in(istream& in)
:i(in)
{
	cinfo.err = jumpy_error_manager(&jerr);

	jmp_buf env;
	cinfo.client_data = &env;

	//Catch "exceptions" and throw proper exceptions
	if(setjmp(env))
	{
		//longjmp called
		char buffer[JMSG_LENGTH_MAX];
		(cinfo.err->format_message)((jpeg_common_struct*)&cinfo, buffer);
		throw CVD::Exceptions::Image_IO::MalformedImage(string("Error in JPEG image: ") + buffer);
	}
	
	jpeg_create_decompress(&cinfo);
	jpeg_istream_src::create(&cinfo, &i);

	jpeg_read_header(&cinfo, true);
	jpeg_start_decompress(&cinfo);

	xs = cinfo.output_width;
	ys = cinfo.output_height;

	m_channels = cinfo.out_color_components;
}

void jpeg_in::get_raw_pixel_lines(unsigned char*data, unsigned long nlines)
{
	jmp_buf env;
	cinfo.client_data = &env;
	
	//Catch "exceptions" and throw proper exceptions
	if(setjmp(env))
	{
		//longjmp called
		char buffer[JMSG_LENGTH_MAX];
		(cinfo.err->format_message)((jpeg_common_struct*)&cinfo, buffer);
		throw CVD::Exceptions::Image_IO::MalformedImage(string("Error in JPEG image: ") + buffer);
	}

	
	unsigned char** datap = &data;
	for(unsigned int i=0; i < nlines; i++)	
	{
		jpeg_read_scanlines(&cinfo, datap, 1);
		data += elements_per_line();
	}
}

jpeg_in::~jpeg_in()
{
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
}

////////////////////////////////////////////////////////////////////////////////
//
//  Compression 
//

struct jpeg_ostream_dest: public jpeg_destination_mgr
{
	ostream* o;
	static const int bufsize=262144;
	JOCTET buf[bufsize];

	static void create(j_compress_ptr p, ostream* os)
	{
		//Allocate memory
		p->dest = (jpeg_ostream_dest*) (*p->mem->alloc_small)((jpeg_common_struct*)p, JPOOL_PERMANENT, sizeof(jpeg_ostream_dest));

		jpeg_ostream_dest* me = (jpeg_ostream_dest*) p->dest;

		//Set up virtual member functions
		me->init_destination = s_init_destination;
		me->empty_output_buffer = s_empty_output_buffer;
		me->term_destination = s_term_destination;

		//Set up data members
		me->o = os;
	}

	static void s_init_destination(j_compress_ptr cinfo)
	{
		jpeg_ostream_dest* me = (jpeg_ostream_dest*) cinfo->dest;

		me->next_output_byte = &me->buf[0];
		me->free_in_buffer = bufsize;
	}

	static int s_empty_output_buffer(j_compress_ptr cinfo)
	{
		jpeg_ostream_dest* me = (jpeg_ostream_dest*) cinfo->dest;

		//Docs say we should do this: 
		me->o->write((const char*)me->buf, bufsize);

		s_init_destination(cinfo);
		return 1;
	}

	static void s_term_destination(j_compress_ptr cinfo)
	{
		jpeg_ostream_dest* me = (jpeg_ostream_dest*) cinfo->dest;
		me->o->write((const char*)me->buf, bufsize-me->free_in_buffer);
	}
};




jpeg_out::jpeg_out(std::ostream& out, int xsize, int ysize, int try_channels, const string& comm)
:o(out)
{
	xs = xsize;
	ys = ysize;

	if(try_channels < 3)
		m_channels = 1;
	else
		m_channels = 3;
	
	//Set up setjmp/lonjmp error handling
	cinfo.err = jumpy_error_manager(&jerr);
	jmp_buf env;
	cinfo.client_data = &env;

	//Catch "exceptions" and throw proper exceptions
	if(setjmp(env))
	{
		//longjmp called
		char buffer[JMSG_LENGTH_MAX];
		(cinfo.err->format_message)((jpeg_common_struct*)&cinfo, buffer);
		throw CVD::Exceptions::Image_IO::WriteError(string("JPEG: ") + buffer);
	}

	//Create the compression object
	jpeg_create_compress(&cinfo);
	
	//Get the jpeg_ostream_dest class to handle output.
	jpeg_ostream_dest::create(&cinfo, &o);

	//Setup parameters
	cinfo.image_width = xs;
	cinfo.image_height = ys;
	cinfo.input_components = m_channels;
	cinfo.in_color_space = (m_channels==3) ? JCS_RGB : JCS_GRAYSCALE;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 95, TRUE);

	jpeg_start_compress(&cinfo, TRUE);

	unsigned int commlen = comm.length();
	
	//NB: not 65536 Marker looks like:
	// marker_byte length_high length_low length*bytes
	// length includes the block header (3 bytes)	

	if(commlen > 65533)
		commlen = 65533;

	//Written without zero termination, since the length is also written
	jpeg_write_marker(&cinfo, JPEG_COM, (JOCTET*)comm.c_str(), comm.length());
}

void jpeg_out::write_raw_pixel_lines(const unsigned char* data, unsigned long nlines)
{
	jmp_buf env;
	cinfo.client_data = &env;
	long elem = elements_per_line();
	
	//Catch "exceptions" and throw proper exceptions
	if(setjmp(env))
	{
		//longjmp called
		char buffer[JMSG_LENGTH_MAX];
		(cinfo.err->format_message)((jpeg_common_struct*)&cinfo, buffer);
		throw CVD::Exceptions::Image_IO::MalformedImage(string("JPEG: ") + buffer);
	}

	const unsigned char** datap = &data;
	for(unsigned int i=0; i < nlines; i++)	
	{
		jpeg_write_scanlines(&cinfo, (JSAMPLE**)datap, 1);
		data += elem;
	}
}

jpeg_out::~jpeg_out()
{
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
}


}
}
