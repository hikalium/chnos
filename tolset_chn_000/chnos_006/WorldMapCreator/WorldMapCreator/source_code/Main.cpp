/***********************************************************
  ���̃\�t�g��DX���C�u������VisualStudio2005�ō���Ă܂�
***********************************************************/

#include "DxLib.h"

#define BMP_SIZE_X	340
#define BMP_SIZE_Y	350

#define BMP_FILENAME		"map.bmp"
#define CR_DEFINE_FILENAME	"ColorDefine.txt"
#define MAPCODE_FILENAME	"MapCode.txt"

typedef struct {
	int DefCr;
	int	DefNum;
} MapChip_t;

class FileIO {
	int  m_Cr;
	int  m_LineNum;
	MapChip_t ChipInfo[256];
public:
	FileIO				( void ){ m_LineNum = 0; }
	void ReadDefine		( void );
	void ReadBmp		( void );
	void SaveCode		( void );
	
} ;

//-------------------------------------------------------
// ReadDefine Function
//-------------------------------------------------------
void FileIO::ReadDefine( void ){
	int	FileHandle ;
	char Str[256] ;
	// ��`�t�@�C�����J��
	FileHandle = FileRead_open( CR_DEFINE_FILENAME ) ;

	// �t�@�C���̏I�[������܂�
	while( FileRead_eof( FileHandle ) == 0 )
	{
		// ��s�ǂݍ���
		FileRead_gets( Str , 256 , FileHandle ) ;
		
		char tmpStr[256]; 
		int  R, G, B, N;
		for ( int i=0, p=0, t=0; i < 256; i++ )
		{
			tmpStr[p++] = Str[i];
			if ( Str[i] == ',' )
			{
				p = 0;
				switch ( t++ ){
				case 0:
						R = atoi(tmpStr);
					break;
				case 1:
						G = atoi(tmpStr);
					break;
				case 2:
						B = atoi(tmpStr);
					break;
				case 3:
						m_LineNum++;
						N = atoi(tmpStr);
					break;
				}
			}
		}
		ChipInfo[m_LineNum - 1].DefCr  = GetColor(R,G,B);
		ChipInfo[m_LineNum - 1].DefNum = N;
	}
	// �t�@�C�������
	FileRead_close( FileHandle ) ;
}

//-------------------------------------------------------
// ReadBmp Function
//-------------------------------------------------------
void FileIO::ReadBmp( void ){
	// bmp�摜�̕\��
	LoadGraphScreen( 0 , 0 , BMP_FILENAME , TRUE ) ;
	ScreenFlip();
}

//-------------------------------------------------------
// CheckCr Function
//-------------------------------------------------------
bool CheckCr( int cr, int tcr ){
	int r1, g1, b1, r2, g2, b2;
	GetColor2( cr , &r1, &g1, &b1 );
	GetColor2( tcr, &r2, &g2, &b2 );
	int sR, sG, sB;
	r1 > r2 ? sR=r1-r2 : sR=r2-r1;
	g1 > g2 ? sG=g1-g2 : sG=g2-g1;
	b1 > b2 ? sB=b1-b2 : sB=b2-b1;
	enum { RED, GREEN, BLUE };
	bool FlagSame[3] = {0};
	// �e�l�덷���}8�ȓ��Ȃ瓙�����Ƃ��Ĉ���
	if ( sR <= 16 ) FlagSame[RED]   = true;
	if ( sG <= 16 ) FlagSame[GREEN] = true;
	if ( sB <= 16 ) FlagSame[BLUE]  = true;

	if ( FlagSame[RED] && FlagSame[GREEN] && FlagSame[BLUE] )
	{
		return true;
	}
	return false;
}

//-------------------------------------------------------
// SaveCode Function
//-------------------------------------------------------
void FileIO::SaveCode( void ){
	FILE* stream;
	// ��`�t�@�C�����J��
	fopen_s( &stream, MAPCODE_FILENAME, "w+");

	for ( int j = 0; j < BMP_SIZE_Y; j++ )
	{
		for ( int i = 0; i < BMP_SIZE_X; i++ )
		{
			bool FlagFind = false;
			m_Cr = GetPixel( i , j ) ;
			for ( int t = 0; t < m_LineNum; t++ )
			{
				if ( m_Cr == ChipInfo[t].DefCr || 
					CheckCr( m_Cr, ChipInfo[t].DefCr ) == true  )
				{
					FlagFind = true;
					
					char tStr[80], buf[80];
					if ( ChipInfo[t].DefNum < 10 ){
						strcpy_s( tStr, 80, "  " );
						_itoa_s( ChipInfo[t].DefNum, buf, 80, 10 );
						strcat_s( tStr, 80, buf );
					} else if ( ChipInfo[t].DefNum < 100 ){
						strcpy_s( tStr, 80, " " );
						_itoa_s( ChipInfo[t].DefNum, buf, 80, 10 );
						strcat_s( tStr, 80, buf );
					} else {
						strcpy_s( tStr, 80, "" );
						_itoa_s( ChipInfo[t].DefNum, buf, 80, 10 );
						strcat_s( tStr, 80, buf );
					}
					fputs( tStr, stream );
					break;
				}
			}
			// ��`��������Ȃ������ꍇ��-1����������
			if ( FlagFind == false )
			{
				fputs( " -1", stream );
			}
		}
		fputs( "\n", stream );
	}

	// �t�@�C�������
	fclose( stream );
}

//////////////////////////////
//   W i n M a i n
//////////////////////////////
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
						LPSTR lpCmdLine, int nCmdShow )
{
	// �^�C�g���̕ύX
	SetMainWindowText( "World Map Creator" );
	// �E�C���h�E���[�h�ɕύX
	ChangeWindowMode( TRUE );
	// �c�w���C�u�������O�̔�o��
	SetOutApplicationLogValidFlag( FALSE );
	// �c�w���C�u��������������
	if( DxLib_Init() == -1 )	
	{
		 return -1;
	}
	// �`����ʂ𗠉�ʂɂ���
	SetDrawScreen( DX_SCREEN_BACK ) ;
	
	{
		FileIO ob1;
		ob1.ReadDefine();
		ob1.ReadBmp();
		ob1.SaveCode();
	}
	printfDx( MAPCODE_FILENAME "�ւ̏������ݏI���I");
	ScreenFlip();
	WaitKey();
	// �c�w���C�u�����g�p�̏I������
	DxLib_End() ;			
	// �\�t�g�̏I�� 
	return 0 ;				
}