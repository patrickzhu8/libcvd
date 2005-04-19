#ifndef CVD_GL_HELPERS_H
#define CVD_GL_HELPERS_H


#include <cvd/image_ref.h>
#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>
#include <cvd/byte.h>
#include <cvd/rgb8.h>
#include <cvd/rgba.h>
#include <numerics.h>
#include <GL/gl.h>

namespace CVD
{
	
	namespace gl
	{
		template<class C> struct data;

		//Scalar types

		template<> struct data<unsigned char>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_UNSIGNED_BYTE;
		};

		template<> struct data<signed char>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_BYTE;
		};
		
		template<> struct data<unsigned  short>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_UNSIGNED_SHORT;
		};

		template<> struct data<short>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_SHORT;
		};

		template<> struct data<unsigned int>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_UNSIGNED_INT;
		};

		template<> struct data<int>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_INT;
		};
		
		template<> struct data<float>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_FLOAT;
		};
	

		//Rgb<*> types
	
		template<> struct data<Rgb<unsigned char> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_UNSIGNED_BYTE;
		};

		template<> struct data<Rgb<signed char> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_BYTE;
		};
		
		template<> struct data<Rgb<unsigned  short> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_UNSIGNED_SHORT;
		};

		template<> struct data<Rgb<short> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_SHORT;
		};

		template<> struct data<Rgb<unsigned int> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_UNSIGNED_INT;
		};

		template<> struct data<Rgb<int> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_INT;
		};
		
		template<> struct data<Rgb<float> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_FLOAT;
		};
		
		//Rgba<*> types
	
		template<> struct data<Rgba<unsigned char> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_UNSIGNED_BYTE;
		};

		template<> struct data<Rgba<signed char> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_BYTE;
		};
		
		template<> struct data<Rgba<unsigned  short> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_UNSIGNED_SHORT;
		};

		template<> struct data<Rgba<short> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_SHORT;
		};

		template<> struct data<Rgba<unsigned int> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_UNSIGNED_INT;
		};

		template<> struct data<Rgba<int> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_INT;
		};
		
		template<> struct data<Rgba<float> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_FLOAT;
		};
		//Rgb8 type

		template<> struct data<Rgb8>
		{
			static const int format=GL_RGBA;
			static const int type  =GL_UNSIGNED_BYTE;
		};
		
	};

	
	/// Specify the (x,y) co-ordinates of a vertex
	/// @param i The vertex location
	///@ingroup gGL
	inline void glVertex(const ImageRef& i)
	{
		glVertex2i(i.x, i.y);
	}

	/// Specify the (x,y) co-ordinates of a vertex
	/// @param v The vertex location
	///@ingroup gGL
	inline void glVertex(const Vector<2>& v)
	{
		glVertex2d(v[0], v[1]);
	}

	/// Specify the (x,y,z) co-ordinates of a vertex
	/// @param v The vertex location
	///@ingroup gGL
	inline void glVertex(const Vector<3>& v)
	{
		glVertex3d(v[0], v[1], v[2]);
	}

	/// Specify the (x,y,z,w) co-ordinates of a vertex
	/// @param v The vertex location
	///@ingroup gGL
	inline void glVertex(const Vector<4>& v)
	{
		glVertex4d(v[0], v[1], v[2], v[3]);
	}




	/// Set the new colour to the red, green and blue components given in the Vector 
	/// (where 0.0 represents zero intensity and 1.0 full intensity)
	/// @param v The new colour
	///@ingroup gGL
	inline void glColor(const Vector<3>& v)
	{
		glColor3d(v[0], v[1], v[2]);
	}

	/// Set the new colour to the red, green, blue and alpha components given in the Vector 
	/// (where 0.0 represents zero intensity and 1.0 full intensity)
	/// @param v The new colour
	///@ingroup gGL
	inline void glColor(const Vector<4>& v)
	{
		glColor4d(v[0], v[1], v[2], v[3]);
	}

	/// Set the new colour to the red, green, blue components given
	/// (where 0 represents zero intensity and 255 full intensity)
	/// @param c The new colour
	///@ingroup gGL
	inline void glColor(const CVD::Rgb<byte>& c)
	{
		glColor3ub(c.red, c.green, c.blue);
	}

 	/// Set the new colour to the red, green and blue components given
	/// (where 0.0 represents zero intensity and 1.0 full intensity)
	/// @param c The new colour
	///@ingroup gGL
	inline void glColor(const CVD::Rgb<float>& c)
	{
		glColor3f(c.red, c.green, c.blue);
	}

 	/// Set the new colour to the red, green and blue components given
	/// (where 0.0 represents zero intensity and 1.0 full intensity). The Rgb8::dummy member is ignored
	/// @param c The new colour
	///@ingroup gGL
	inline void glColor3(const CVD::Rgb8& c)
	{
		glColor3ub(c.red, c.green, c.blue);
	}
	
 	/// Set the new colour to the red, green, blue and alpha components given
	/// (where 0.0 represents zero intensity and 1.0 full intensity)
	/// @param c The new colour
	///@ingroup gGL
	inline void glColor4(const CVD::Rgb8& c)
	{
		glColor4ub(c.red, c.green, c.blue, c.dummy);
	}

 	/// Set the new colour to the red, green, blue and alpha components given
	/// (where 0 represents zero intensity and 255 full intensity)
	/// @param c The new colour
	///@ingroup gGL
	inline void glColor(const CVD::Rgba<unsigned char>& c)
	{
		glColor4ub(c.red, c.green, c.blue, c.alpha);
	}

 	/// Set the new colour to the red, green, blue and alpha components given
	/// (where 0.0 represents zero intensity and 1.0 full intensity)
	/// @param c The new colour
	///@ingroup gGL
	inline void glColor(const CVD::Rgba<float>& c)
	{
		glColor4f(c.red, c.green, c.blue, c.alpha);
	}

 	/// Draw an image to the frame buffer at the current raster position. 
	/// Use glRasterPos to set the current raster position
	/// @param i The image to draw
	///@ingroup gGL
	template<class C> inline void glDrawPixels(const BasicImage<C>& i)
	{
		::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		::glDrawPixels(i.size().x, i.size().y, gl::data<C>::format, gl::data<C>::type, i.data());
	}

 	/// Read the current image from the colour buffer specified by glReadBuffer
	/// @param i The image to write the image data into. This must already be initialised to be an BasicImage (or Image) of the right size.
	/// @param origin The window co-ordinate of the first pixel to be read from the frame buffer
	///@ingroup gGL
	template<class C> inline void glReadPixels(BasicImage<C>& i, ImageRef origin=ImageRef(0,0))
	{
		::glReadPixels(origin.x, origin.y, i.size().x, i.size().y, gl::data<C>::format, gl::data<C>::type, i.data());
	}

};

#endif
