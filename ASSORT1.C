#include "dos.h"
#include "stdio.h"

main()
{
	struct boot
	{
		unsigned char jump[3] ;
		char system_id[8] ;
		int bytes_per_sec ;
		unsigned char sec_per_clus ;
		int res_sec ;
		unsigned char fat_copies ;
		int root_dir_entry ;
		unsigned int no_sects ;
		unsigned char format_id ;
		int sec_per_fat ;
		int sec_per_trk ;
		int no_sides ;
		int no_sp_res_sect ;
		char rest_code[482] ;
	} ;

	struct boot b ;
	int drive, i ;

	clrscr() ;

	printf ( "Enter drive no. 0 = A, 1 = B, 2 = C, ..." ) ;
	scanf ( "%d", &drive ) ;

	absread ( drive, 1, 0, &b ) ;

	printf ( "\n\nJump instruction" ) ;
	for ( i = 0 ; i < 3 ; i++ )
		printf ( " %X", b.jump[i] ) ;

	printf ( "\nSystem ID %s\n", b.system_id ) ;
	printf ( "Bytes per sector %d\n", b.bytes_per_sec ) ;
	printf ( "Sectors per cluster %d\n", b.sec_per_clus ) ;
	printf ( "Reserved sectors %d\n", b.res_sec ) ;
	printf ( "FAT copies %d\n", b.fat_copies ) ;
	printf ( "Root directory entries %d\n", b.root_dir_entry ) ;
	printf ( "No. of sectors on disk %u\n", b.no_sects ) ;
	printf ( "Format ID %X\n", b.format_id ) ;
	printf ( "Sectors per FAT %d\n", b.sec_per_fat ) ;
	printf ( "Sectors per track %d\n", b.sec_per_trk ) ;
	printf ( "No of sides %d\n", b.no_sides ) ;
	printf ( "No of reserved sectors %d\n", b.no_sp_res_sect ) ;
}
