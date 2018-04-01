#if defined(_WIN64)
extern VOID AlignRSP( VOID );

VOID Begin( VOID )
{
	AlignRSP();
}
#endif