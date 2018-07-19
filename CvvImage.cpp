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
}//���ϣ�����һ��RECT���������������ҳߴ���ȷ
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
}//��cv��ת��mfc�Ͳ�����
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
}//��ȡROI
void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin )
{
   assert( bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));//������bmiָ����Ч������ֵ��Ч��ͼ����ȣ�ÿ�����ش洢��ɫ�õ�λ�ô�С��Ϊ8����������ɫ���߲�ɫ�����ɫ��
   BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);//����һ��ָ��bmiͷ��ַ��ָ��

   memset( bmih, 0, sizeof(*bmih));//����ָ����������
   bmih->biSize = sizeof(BITMAPINFOHEADER);//��С����������Ĭ��
   bmih->biWidth = width;
   bmih->biHeight = origin ? abs(height) : -abs(height);
   bmih->biPlanes = 1;//����ڰ�ͼ��
   bmih->biBitCount = (unsigned short)bpp;
   bmih->biCompression = BI_RGB;//ѹ��������Ӧ����ȫ�����ȶ���
   if( bpp == 8 )
   {
      RGBQUAD* palette = bmi->bmiColors;//������ɫ��ָ�룬ָ��bmi����ɫ�壬256��ɫ�����в�ͬ������RGB
      int i;
      for( i = 0; i < 256; i++ )
      {
         palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;//������ȫ����ͬ��iֵ����Ϊ�Ҷ�ͼ
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
bool  CvvImage::Create( int w, int h, int bpp, int origin )//z���ͼ���������ĶԲ���
{
   const unsigned max_img_size = 10000;//���ͼ��ߴ�

   if( (bpp != 8 && bpp != 24 && bpp != 32) ||
      (unsigned)w >=  max_img_size || (unsigned)h >= max_img_size ||
      (origin != IPL_ORIGIN_TL && origin != IPL_ORIGIN_BL))
   {
      assert(0); // most probably, it is a programming error
      return false;
   }//������ʾ
   if( !m_img || Bpp() != bpp || m_img->width != w || m_img->height != h )
   {
      if( m_img && m_img->nSize == sizeof(IplImage))
         Destroy();//����ˣ����٣�����
      /* prepare IPL header */
      m_img = cvCreateImage( cvSize( w, h ), IPL_DEPTH_8U, bpp/8 );
   }
   if( m_img )
      m_img->origin = origin == 0 ? IPL_ORIGIN_TL : IPL_ORIGIN_BL;
   return m_img != 0;//��m_img��=0�����صľ���1true
}
void  CvvImage::CopyOf( CvvImage& image, int desired_color )
{
   IplImage* img = image.GetImage();//�õ�CvvImageͼ���Iplָ��
   if( img )
   {
      CopyOf( img, desired_color );
   }
}
#define HG_IS_IMAGE(img)                                                  \
   ((img) != 0 && ((const IplImage*)(img))->nSize == sizeof(IplImage) && \
   ((IplImage*)img)->imageData != 0)
void  CvvImage::CopyOf( IplImage* img, int desired_color )//ת�����ƣ��������img����ɫҪ���Ƶ�m_img
{
   if( HG_IS_IMAGE(img) )//img��������
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
bool  CvvImage::Load( const char* filename, int desired_color )//���أ����ļ��а���ɫҪ�����ͼ��
{
   IplImage* img = cvLoadImage( filename, desired_color );//��ͼ����ص�imgָ����
   if( !img )
      return false;

   CopyOf( img, desired_color );//����ָ���ڵ�ͼ����ɺ�m_img����ж����ˣ�
   cvReleaseImage( &img );//�ͷ�ָ�룬be a good boy

   return true;
}
bool  CvvImage::LoadRect( const char* filename,
                   int desired_color, CvRect r )//���ؾ��μ�ͼƬ���趨ROI
{
   if( r.width < 0 || r.height < 0 ) return false;

   IplImage* img = cvLoadImage( filename, desired_color );
   if( !img )
      return false;
   if( r.width == 0 || r.height == 0 )//��û���þ��γ�/����Ĭ��ȡͼ���С�ľ���
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
   }//���β���
   /* truncate r to source image */
   if( r.x < 0 )//�������ԭ�����⡣��ԭ��Ų����ʾ��ԭ�㣬����ͼ�񳬳���ʾ����Χ����Ĩȥ
   {
      r.width += r.x;
      r.x = 0;
   }
   if( r.y < 0 )
   {
      r.height += r.y;
      r.y = 0;
   }
   if( r.x + r.width > img->width )//������ǳ���Χ����������
      r.width = img->width - r.x;

   if( r.y + r.height > img->height )
      r.height = img->height - r.y;
   cvSetImageROI( img, r );
   CopyOf( img, desired_color );
   cvReleaseImage( &img );
   return true;
}
bool  CvvImage::Save( const char* filename )//��ͼ������ļ���
{
   if( !m_img )
      return false;
   cvSaveImage( filename, m_img );
   return true;
}
void  CvvImage::Show( const char* window )//��ʾͼ�����ض����ڣ�m_img��
{
   if( m_img )
      cvShowImage( window, m_img );
}
void  CvvImage::Show( HDC dc, int x, int y, int w, int h, int from_x, int from_y )//��ʾ���������豸DC
{
   if( m_img && m_img->depth == IPL_DEPTH_8U )
   {
      uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];//�������Ŀռ䣬�����ݴ�λͼ�ṹ����Ϣ
      BITMAPINFO* bmi = (BITMAPINFO*)buffer;//����λͼ��Ϣָ�룬ָ��buffer���ٵĿռ�
      int bmp_w = m_img->width, bmp_h = m_img->height;//���û�ȡm_img����
      FillBitmapInfo( bmi, bmp_w, bmp_h, Bpp(), m_img->origin );
      from_x = MIN( MAX( from_x, 0 ), bmp_w - 1 );//����ԭ��
      from_y = MIN( MAX( from_y, 0 ), bmp_h - 1 );
      int sw = MAX( MIN( bmp_w - from_x, w ), 0 );
      int sh = MAX( MIN( bmp_h - from_y, h ), 0 );
      SetDIBitsToDevice(
         dc, x, y, sw, sh, from_x, from_y, from_y, sh,
         m_img->imageData + from_y*m_img->widthStep,
         bmi, DIB_RGB_COLORS );//������ʾ��DC
   }
}
void  CvvImage::DrawToHDC( HDC hDCDst, RECT* pDstRect ) //��ͼ���豸�ϡ�����ͷ�豸�����о���
{
   if( pDstRect && m_img && m_img->depth == IPL_DEPTH_8U && m_img->imageData )//
   {
      uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
      BITMAPINFO* bmi = (BITMAPINFO*)buffer;
      int bmp_w = m_img->width, bmp_h = m_img->height;
      CvRect roi = cvGetImageROI( m_img );//����ͼ���ROI���ꣻ���û��ROI���򷵻ؾ���ֵ(cvRect(0,0,image->width,image->height))
      CvRect dst = RectToCvRect( *pDstRect );//��RECTתCvRect
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

