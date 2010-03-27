/***********************************************************
  このソフトはDXライブラリとVisualStudio2005で作られてます
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
	// 定義ファイルを開く
	FileHandle = FileRead_open( CR_DEFINE_FILENAME ) ;

	// ファイルの終端が来るまで
	while( FileRead_eof( FileHandle ) == 0 )
	{
		// 一行読み込み
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
	// ファイルを閉じる
	FileRead_close( FileHandle ) ;
}

//-------------------------------------------------------
// ReadBmp Function
//-------------------------------------------------------
void FileIO::ReadBmp( void ){
	// bmp画像の表示
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
	// 各値誤差が±8以内なら等しいとして扱う
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
	// 定義ファイルを開く
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
			// 定義が見つからなかった場合は-1を書き込む
			if ( FlagFind == false )
			{
				fputs( " -1", stream );
			}
		}
		fputs( "\n", stream );
	}

	// ファイルを閉じる
	fclose( stream );
}

//////////////////////////////
//   W i n M a i n
//////////////////////////////
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
						LPSTR lpCmdLine, int nCmdShow )
{
	// タイトルの変更
	SetMainWindowText( "World Map Creator" );
	// ウインドウモードに変更
	ChangeWindowMode( TRUE );
	// ＤＸライブラリログの非出力
	SetOutApplicationLogValidFlag( FALSE );
	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 )	
	{
		 return -1;
	}
	// 描画先画面を裏画面にする
	SetDrawScreen( DX_SCREEN_BACK ) ;
	
	{
		FileIO ob1;
		ob1.ReadDefine();
		ob1.ReadBmp();
		ob1.SaveCode();
	}
	printfDx( MAPCODE_FILENAME "への書き込み終了！");
	ScreenFlip();
	WaitKey();
	// ＤＸライブラリ使用の終了処理
	DxLib_End() ;			
	// ソフトの終了 
	return 0 ;				
}