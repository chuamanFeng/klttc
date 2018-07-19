#include "StdAfx.h"
#include "CvvImage.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CV_INLINE RECT NormalizeRect( RECT r );
CV_INLINE RECT NormalizeRect( RECT r )
{
   int t;
   if( r.left > r.right )
   {
      t = r.left;
      r.left = r.right;
      r.right = t;
   }
   if( r.top > r.bottom )
   {
      t = r.top;
      r.top = r.bottom;
      r.bottom = t;
   }

   return r;
}//以上：输入一个RECT，调整其上下左右尺寸正确
CV_INLINE CvRect RectToCvRect( RECT sr );
CV_INLINE CvRect RectToCvRect( RECT sr )
{
   sr = NormalizeRect( sr );
   return cvRect( sr.left, sr.top, sr.right - sr.left, sr.bottom - sr.top );
}
CV_INLINE RECT CvRectToRect( CvRect sr );
CV_INLINE RECT CvRectToRect( CvRect sr )
{
   RECT dr;
   dr.left = sr.x;
   dr.top = sr.y;
   dr.right = sr.x + sr.width;
   dr.bottom = sr.y + sr.height;

   return dr;
}//将cv型转至mfc型并返回
CV_INLINE IplROI RectToROI( RECT r );
CV_INLINE IplROI RectToROI( RECT r )
{
   IplROI roi;
   r = NormalizeRect( r );
   roi.xOffset = r.left;
   roi.yOffset = r.top;
   roi.width = r.right - r.left;
   roi.height = r.bottom - r.top;
   roi.coi = 0;

   return roi;
}//截取ROI
void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin )
{
   assert( bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));//声明：bmi指针有效，长宽值有效，图像深度（每个像素存储颜色用的位置大小）为8整数倍（彩色，高彩色，真彩色）
   BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);//重设一个指向bmi头地址的指针

   memset( bmih, 0, sizeof(*bmih));//将该指针内容设零
   bmih->biSize = sizeof(BITMAPINFOHEADER);//大小等于数据型默认
   bmih->biWidth = width;
   bmih->biHeight = origin ? abs(height) : -abs(height);
   bmih->biPlanes = 1;//单层黑白图像
   bmih->biBitCount = (unsigned short)bpp;
   bmih->biCompression = BI_RGB;//压缩参数。应该在全局有先定义
   if( bpp == 8 )
   {
      RGBQUAD* palette = bmi->bmiColors;//建立调色板指针，指向bmi的颜色板，256个色，各有不同比例的RGB
      int i;
      for( i = 0; i < 256; i++ )
      {
         palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;//把它们全部等同于i值，化为灰度图
         palette[i].rgbReserved = 0;
      }
   }
}
CvvImage::CvvImage()
{
   m_img = 0;//IplImage*
}
void CvvImage::Destroy()
{
   cvReleaseImage( &m_img );
}
CvvImage::~CvvImage()
{
   Destroy();
}
bool  CvvImage::Create( int w, int h, int bpp, int origin )//z造个图，并检查造的对不对
{
   const unsigned max_img_size = 10000;//最大图像尺寸

   if( (bpp != 8 && bpp != 24 && bpp != 32) ||
      (unsigned)w >=  max_img_size || (unsigned)h >= max_img_size ||
      (origin != IPL_ORIGIN_TL && origin != IPL_ORIGIN_BL))
   {
      assert(0); // most probably, it is a programming error
      return false;
   }//出错提示
   if( !m_img || Bpp() != bpp || m_img->width != w || m_img->height != h )
   {
      if( m_img && m_img->nSize == sizeof(IplImage))
         Destroy();//造错了，销毁，辣鸡
      /* prepare IPL header */
      m_img = cvCreateImage( cvSize( w, h ), IPL_DEPTH_8U, bpp/8 );
   }
   if( m_img )
      m_img->origin = origin == 0 ? IPL_ORIGIN_TL : IPL_ORIGIN_BL;
   return m_img != 0;//若m_img！=0，返回的就是1true
}
void  CvvImage::CopyOf( CvvImage& image, int desired_color )
{
   IplImage* img = image.GetImage();//得到CvvImage图像的Ipl指针
   if( img )
   {
      CopyOf( img, desired_color );
   }
}
#define HG_IS_IMAGE(img)                                                  \
   ((img) != 0 && ((const IplImage*)(img))->nSize == sizeof(IplImage) && \
   ((IplImage*)img)->imageData != 0)
void  CvvImage::CopyOf( IplImage* img, int desired_color )//转换复制，将输入的img依颜色要求复制到m_img
{
   if( HG_IS_IMAGE(img) )//img符合条件
   {
      int color = desired_color;//Good habit
      CvSize size = cvGetSize( img ); 
      if( color < 0 )
         color = img->nChannels > 1;
      if( Create( size.width, size.height,
         (!color ? 1 : img->nChannels > 1 ? img->nChannels : 3)*8,
         img->origin ))
      {
         cvConvertImage( img, m_img, 0 );
      }
   }
}
bool  CvvImage::Load( const char* filename, int desired_color )//加载：从文件中按颜色要求加载图像
{
   IplImage* img = cvLoadImage( filename, desired_color );//把图像加载到img指针内
   if( !img )
      return false;

   CopyOf( img, desired_color );//复制指针内的图像（完成后m_img里就有东西了）
   cvReleaseImage( &img );//释放指针，be a good boy

   return true;
}
bool  CvvImage::LoadRect( const char* filename,
                   int desired_color, CvRect r )//加载矩形及图片，设定ROI
{
   if( r.width < 0 || r.height < 0 ) return false;

   IplImage* img = cvLoadImage( filename, desired_color );
   if( !img )
      return false;
   if( r.width == 0 || r.height == 0 )//若没设置矩形长/宽，则默认取图像大小的矩形
   {
      r.width = img->width;
      r.height = img->height;
      r.x = r.y = 0;
   }
   if( r.x > img->width || r.y > img->height ||
      r.x + r.width < 0 || r.y + r.height < 0 )
   {
      cvReleaseImage( &img );
      return false;
   }//矩形不对
   /* truncate r to source image */
   if( r.x < 0 )//处理矩形原点问题。将原点挪至显示区原点，并将图像超出显示区范围部分抹去
   {
      r.width += r.x;
      r.x = 0;
   }
   if( r.y < 0 )
   {
      r.height += r.y;
      r.y = 0;
   }
   if( r.x + r.width > img->width )//如果还是超范围，则再消减
      r.width = img->width - r.x;

   if( r.y + r.height > img->height )
      r.height = img->height - r.y;
   cvSetImageROI( img, r );
   CopyOf( img, desired_color );
   cvReleaseImage( &img );
   return true;
}
bool  CvvImage::Save( const char* filename )//将图像存入文件中
{
   if( !m_img )
      return false;
   cvSaveImage( filename, m_img );
   return true;
}
void  CvvImage::Show( const char* window )//显示图像于特定窗口（m_img）
{
   if( m_img )
      cvShowImage( window, m_img );
}
void  CvvImage::Show( HDC dc, int x, int y, int w, int h, int from_x, int from_y )//显示于上下文设备DC
{
   if( m_img && m_img->depth == IPL_DEPTH_8U )
   {
      uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];//缓冲器的空间，用于暂存位图结构的信息
      BITMAPINFO* bmi = (BITMAPINFO*)buffer;//创建位图信息指针，指向buffer开辟的空间
      int bmp_w = m_img->width, bmp_h = m_img->height;//设置获取m_img长宽
      FillBitmapInfo( bmi, bmp_w, bmp_h, Bpp(), m_img->origin );
      from_x = MIN( MAX( from_x, 0 ), bmp_w - 1 );//设置原点
      from_y = MIN( MAX( from_y, 0 ), bmp_h - 1 );
      int sw = MAX( MIN( bmp_w - from_x, w ), 0 );
      int sh = MAX( MIN( bmp_h - from_y, h ), 0 );
      SetDIBitsToDevice(
         dc, x, y, sw, sh, from_x, from_y, from_y, sh,
         m_img->imageData + from_y*m_img->widthStep,
         bmi, DIB_RGB_COLORS );//将其显示于DC
   }
}
void  CvvImage::DrawToHDC( HDC hDCDst, RECT* pDstRect ) //将图像画设备上。输入头设备，还有矩形
{
   if( pDstRect && m_img && m_img->depth == IPL_DEPTH_8U && m_img->imageData )//
   {
      uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
      BITMAPINFO* bmi = (BITMAPINFO*)buffer;
      int bmp_w = m_img->width, bmp_h = m_img->height;
      CvRect roi = cvGetImageROI( m_img );//返回图像的ROI坐标；如果没有ROI，则返回矩形值(cvRect(0,0,image->width,image->height))
      CvRect dst = RectToCvRect( *pDstRect );//将RECT转CvRect
      if( roi.width == dst.width && roi.height == dst.height )
      {
         Show( hDCDst, dst.x, dst.y, dst.width, dst.height, roi.x, roi.y );
         return;
      }
      if( roi.width > dst.width )
      {
         SetStretchBltMode(
            hDCDst,           // handle to device context
            HALFTONE );
      }
      else
      {
         SetStretchBltMode(
            hDCDst,           // handle to device context
            COLORONCOLOR );
      }
      FillBitmapInfo( bmi, bmp_w, bmp_h, Bpp(), m_img->origin );
      ::StretchDIBits(
         hDCDst,
         dst.x, dst.y, dst.width, dst.height,
         roi.x, roi.y, roi.width, roi.height,
         m_img->imageData, bmi, DIB_RGB_COLORS, SRCCOPY );
   }
}
void  CvvImage::Fill( int color )
{
   cvSet( m_img, cvScalar(color&255,(color>>8)&255,(color>>16)&255,(color>>24)&255) );
}

