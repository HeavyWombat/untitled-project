#ifndef TEXT_H_
#define TEXT_H_

void text_init( void );
void text_setDefaults( void );
void text_setDefaultTop( float f );
void text_setDefaultLeft( float f );
void text_setLeftOffset( float f );
void text_setTopOffset( float f );
void text_drawText( int left, char* text );
void text_drawTextLn( int left, char* text );
void text_Ln( void );
void text_nLn( unsigned int n );
void text_resetTextOffset( void );

#endif /*TEXT_H_*/
