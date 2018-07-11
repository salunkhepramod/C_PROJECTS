/* positions cursor at the beginning of the file */
start_file()
{
	int display = YES ;

	/* if first page of the file is being currently displayed */
	if ( curscr == startloc && skip == 0 )
		display = NO ;

	/* reset variables */
	curr = 2 ;
	curc = 1 ;
	logr = 1 ;
	logc = 1 ;
	skip = 0 ;
	curscr = startloc ;
	currow = startloc ;

	/* display first page of file, if necessary */
	if ( display == YES )
	{
		menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
		displayscreen ( curscr ) ;
	}

	/* display current cursor location */
	writerow() ;
	writecol() ;
}

/* positions cursor at the end of the file */
end_file()
{
	char *temp ;
	int i, status ;

	size ( 32, 0 ) ;  /* hide cursor */

	/* count the total number of lines in the file */
	logr = 1 ;
	curr = 2 ;
	temp = startloc ;
	while ( temp != endloc )
	{
		if ( *temp == '\n' )
			curr++ ;
		temp++ ;
	}

	/* set up current screen and current row pointers */
	curscr = endloc ;
	currow = endloc ;

	/* display the last page of the file */
	page_up ( 0 ) ;

	/* position cursor 20 lines after current row */
	for ( i = 0 ; i < 20 ; i++ )
	{
		status = down_line ( 0 ) ;

		/* if end of file is reached */
		if ( status == 1 )
			break ;
	}

	/* position cursor at the end of the last line */
	end_line() ;

	size ( 5, 7 ) ;  /* show cursor */

	/* display current cursor row */
	writerow() ;
}

/* positions cursor in the first row of current screen */
top_screen()
{
	size ( 32, 0 ) ;

	/* go up until the first row is encountered */
	while ( logr != 1 )
		up_line ( 0 ) ;

	/* display current cursor row */
	writerow() ;

	size ( 5, 7 ) ;
}

/* positions cursor in the last row of current screen */
bottom_screen()
{
	int status ;

	size ( 32, 0 ) ;

	/* go down until the last row or end of file is encountered */
	while ( logr != 21 )
	{
		status = down_line ( 0 ) ;

		/* if end of file is reached */
		if ( status == 1 )
			break ;
	}

	writerow() ;
	size ( 5, 7 ) ;
}

/* positions cursor at the beginning of current row */
start_line()
{
	/* if there exist characters to the left of currently displayed line */
	if ( skip != 0 )
	{
		skip = 0 ;
		menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
		displayscreen ( curscr ) ;
	}

	logc = 1 ;
	curc = 1 ;

	/* display current cursor column */
	writecol() ;
}

/* positions cursor at the end of current row */
end_line()
{
	char *temp ;
	int count, display = YES ;

	temp = currow ;
	count = 1 ;

	/* count the number of characters in current line */
	while ( *temp != '\n' )
	{
		/* if end of file is encountered */
		if ( temp >= endloc )
			break ;

		if ( *temp == '\t' )
			count += 8 ;
		else
			count++ ;

		temp++ ;
	}

	/* backtrace across the \t's and spaces which may be present at the end of the line */
	while ( * ( temp - 1 ) == '\t' || * ( temp - 1 ) == ' ' )
	{
		if ( * ( temp - 1 ) == '\t' )
			count -= 8 ;
		else
			count++ ;

		temp-- ;
	}

	/* if the number of characters in the line is less than 78 */
	if ( count <= 78 )
	{
		/* if no characters exist to the left of currently displayed line */
		if ( skip == 0 )
			display = NO ;
		else
			skip = 0 ;

		logc = count ;
	}
	else
	{
		skip = count - 78 ;
		logc = 78 ;
	}

	if ( display == YES )
	{
		menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
		displayscreen ( curscr ) ;
	}

	curc = count ;
	writecol() ;
}

/* positions cursor one word to the left */
word_left()
{
	char *temp ;
	int col, count = 0, condition1, condition2, condition3 ;

	/* increment `temp' to point to character at current cursor location */
	temp = currow ;
	for ( col = 1 ; col < curc ; col++ )
	{
		/* if end of file is encountered */
		if ( temp >= endloc )
			break ;

		if ( *temp == '\t' )
			col += 7 ;

		/* if end of line is encountered before current cursor column */
		if ( *temp == '\n' )
		{
			end_line() ;
			break ;
		}

		temp++ ;
	}

	/* if end of file is encountered */
	if ( temp >= endloc )
		temp-- ;

	/* if characters at current cursor location and to its left are alphanumeric */
	condition1 = isalnum ( *temp ) && isalnum ( * ( temp - 1 ) ) ;

	/* if character at current cursor location is alphanumeric and the previous character is not alphanumeric */
	condition2 = isalnum ( *temp ) && !isalnum ( * ( temp - 1 ) ) ;

	/* if character at current cursor location is not alphanumeric */
	condition3 = !isalnum ( *temp ) ;

	if ( *temp == '\n' )
		temp-- ;

	if ( condition2 )
		temp-- ;

	if ( condition1 )
	{
		/* move left so long as alphanumeric characters are found */
		while ( isalnum ( *temp ) )
		{
			if ( temp == startloc )
				break ;

			temp-- ;
			count++ ;
		}
	}

	if ( condition2 || condition3 )
	{
		/* move left till an alphanumeric character is found */
		while ( ! ( isalnum ( *temp ) ) )
		{
			if ( temp <= startloc )
				break ;

			if ( *temp == '\t' )
				count += 7 ;

			/* if end of previous line is encountered */
			if ( *temp == '\n' )
			{
				/* position cursor at the end of previous line */
				up_line ( 0 ) ;
				end_line() ;

				return ;
			}

			temp-- ;
			count++ ;
		}

		/* move left till a non-alphanumeric character is found */
		while ( isalnum ( *temp ) )
		{
			if ( temp == startloc )
				break ;

			temp-- ;
			count++ ;
		}
	}

	/* if beginning of file is encountered */
	if ( temp == startloc )
	{
		logc = 1 ;
		curc = 1 ;
	}
	else
	{
		logc -= count ;
		curc -= count ;

		/* if screen needs to be scrolled horizontally */
		if ( curc > 78 )
		{
			logc = 78 ;
			skip = curc - 78 ;
			menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
			displayscreen ( curscr ) ;
		}
	}

	writecol() ;
	writerow() ;
}

/* positions cursor one word to the right */
word_right()
{
	char *temp ;
	int col, count = 0 ;

	/* increment `temp' to point to character at current cursor location */
	temp = currow ;
	for ( col = 1 ; col < curc ; col++ )
	{
		/* if end of file is encountered */
		if ( temp >= endloc )
			return ;

		if ( *temp == '\t' )
			col += 7 ;

		/* if end of line is encountered before current cursor column */
		if ( *temp == '\n' )
			break ;

		temp++ ;
	}

	if ( temp >= endloc )
		return ;

	/* if cursor is at the end of current line */
	if ( *temp == '\n' )
	{
		/* continue till an alphanumeric character is found */
		while ( ! ( isalnum ( *temp ) ) )
		{
			/* if end of file is encountered */
			if ( temp >= endloc )
				break ;

			if ( *temp == '\t' )
				count += 7 ;

			/* if end of line is encountered */
			if ( *temp == '\n' )
			{
				/* position cursor in the next line */
				down_line ( 0 ) ;

				/* position cursor at the beginning of the line */
				start_line() ;

				temp = currow ;
				count = 0 ;
				continue ;
			}

			temp++ ;
			count++ ;
		}
	}
	else
	{
		/* there exists a word to the right of cursor */
		count = 0 ;

		/* move right so long as alphanumeric characters are found */
		while ( isalnum ( *temp ) )
		{
			if ( temp >= endloc )
				break ;

			temp++ ;
			count++ ;
		}

		/* move right till a non-alphanumeric character or end of line is met */
		while ( ! ( isalnum ( *temp ) || *temp == '\n' ) )
		{
			if ( temp >= endloc )
				break ;

			if ( *temp == '\t' )
				count += 7 ;

			temp++ ;
			count++ ;
		}
	}

	logc += count ;
	curc += count ;

	/* if screen needs to be scrolled horizontally */
	if ( curc > 78 )
	{
		logc = 78 ;
		skip = curc - 78 ;
		menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
		displayscreen ( curscr ) ;
	}

	writecol() ;
	writerow() ;
}

/* displays previous file page */
page_up ( int display )
{
	int row ;

	/* if first page is currently displayed */
	if ( curscr == startloc )
		return ;

	/* position the `curscr' pointer 20 lines before */
	for ( row = 1 ; row <= 20 ; row++ )
	{
		/* go to end of previous line */
		curscr -= 2 ;

		/* if beginning of file is encountered */
		if ( curscr <= startloc )
		{
			/* reset variables */
			curscr = startloc ;
			currow = startloc ;
			logr = 1 ;
			curr = 2 ;
			logc = 1 ;
			curc = 1 ;

			menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
			displayscreen ( curscr ) ;

			if ( display )
			{
				writecol() ;
				writerow() ;
			}

			return ;
		}

		/* go to the beginning of previous line */
		while ( *curscr != '\n' )
		{
			if ( curscr <= startloc )
			{
				curscr = startloc ;
				break ;
			}

			curscr-- ;
		}
		if ( ! ( curscr == startloc || *( curscr + 1 ) == '\n' ) )
			curscr++ ;
	}

	/* display the previous screen */
	menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
	displayscreen ( curscr ) ;

	/* position cursor 20 lines before */
	for ( row = 1 ; row <= 20 ; row++ )
	{
		currow -= 2 ;
		if ( currow < startloc )
			currow = startloc ;

		while ( *currow != '\n' )
			currow-- ;

		if ( currow != startloc || * ( currow + 1 ) == '\n' )
			currow++ ;
	}

	curr -= 20 ;

	/* position cursor in appropriate column */
	gotocol() ;

	if ( display )
	{
		writerow() ;
		writecol() ;
	}
}

/* displays next file page */
page_down()
{
	char *p ;
	int row = 1, i, col ;

	/* position the `curscr' pointer 20 lines hence */
	p = curscr ;
	for ( row = 1 ; row <= 20 ; row++ )
	{
		/* go to the end of current line */
		while ( *curscr != '\n' )
		{
			/* if end of file is encountered */
			if ( curscr >= endloc )
			{
				curscr = p ;
				return ;
			}

			curscr++ ;
		}

		if ( curscr >= endloc )
		{
			curscr = p ;
			return ;
		}

		/* go to the beginning of next line */
		curscr++ ;
	}

	/* display the next screen */
	menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
	displayscreen ( curscr ) ;

	/* position cursor 20 lines hence */

	size ( 32, 0 ) ;

	/* continue till first row on the screen is reached */
	row = 1 ;
	while ( currow != curscr )
	{
		if ( *currow == '\n' )
		{
			curr++ ;
			row++ ;
		}
		currow++ ;
	}

	logr = 1 ;
	col = curc ;

	for ( i = row ; i <= 20 ; i++ )
		down_line ( 0 ) ;

	curc = col ;

	/* position cursor in appropriate column */
	gotocol() ;

	writerow() ;
	writecol() ;

	size ( 5, 7 ) ;
}

/* positions cursor one column to the right */
right()
{
	char *temp ;
	int col ;

	/* if current column exceeds 249, beep */
	if ( curc >= 249 )
	{
		curc = 249 ;
		printf ( "\a" ) ;
		return ;
	}

	/* increment `temp' to point to character at current cursor location */
	temp = currow ;
	for ( col = 1 ; col < curc ; col++ )
	{
		if ( *temp == '\t' )
			col += 7 ;

		if ( *temp == '\n' )
			break ;

		temp++ ;
	}

	/* if next character is a tab */
	if ( *temp == '\t' )
	{
		logc += 7 ;
		curc += 7 ;
	}

	curc++ ;

	/* if cursor is in the last column, scroll screen horizontally */
	if ( logc >= 78 )
	{
		skip = curc - 78 ;
		menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
		displayscreen ( curscr ) ;
		logc = 78 ;
	}
	else
		logc++ ;

	writecol() ;
}

/* positions cursor one column to the left */
left()
{
	int col = 1 ;
	char *temp ;

	/* if cursor is in the first column */
	if ( curc == 1 )
		return ;

	/* increment `temp' to point to character at current cursor location */
	temp = currow ;
	for ( col = 1 ; col < curc ; col++ )
	{
		if ( *temp == '\t' )
			col += 7 ;

		if ( *temp == '\n' )
			break ;

		temp++ ;
	}

	/* if previous character is a tab */
	if ( * ( temp - 1 ) == '\t' )
	{
		logc -= 7 ;
		curc -= 7 ;
	}

	/* if cursor is in the first column and if there exist characters to the left of currently displayed line, scroll screen horizontally */
	if ( logc <= 1 && skip != 0 )
	{
		logc = 1 ;
		curc-- ;
		skip = curc - logc ;
		menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
		displayscreen ( curscr ) ;
	}
	else
	{
		curc-- ;
		logc-- ;
	}

	writecol() ;
}

/* positions cursor one line up */
up_line ( int display )
{
	/* if cursor is in the first line of the file */
	if ( curr == 2 )
		return ;

	/* remove spaces and tabs at the end of the line */
	del_whitespace() ;

	logr-- ;
	curr-- ;

	/* go to the beginning of previous line */
	currow -= 2 ;
	if ( curscr < startloc )
		curscr = startloc ;
	while ( *currow != '\n' )
	{
		if ( currow <= startloc )
			break ;

		currow-- ;
	}
	if ( currow != startloc || *( currow + 1 ) == '\n' )
		currow++ ;

	/* if vertical scrolling is required */
	if ( logr < 1 )
	{
		logr = 1 ;
		curscr = currow ;
		scrolldown ( 2, 1, 22, 78 ) ;
		displayline ( curscr, 2 ) ;
	}

	/* position cursor in appropriate column */
	gotocol() ;

	/* if current cursor row and column is to be displayed */
	if ( display )
	{
		writecol() ;
		writerow() ;
	}
}

/* positions cursor one line down */
down_line ( int display )
{
	char *p ;

	/* remove spaces and tabs at the end of the line */
	del_whitespace() ;

	/* go to the beginning of next line */
	p = currow ;
	while ( *currow != '\n' )
	{
		/* if end of file is encountered */
		if ( currow >= endloc )
		{
			currow = p ;
			return ( 1 ) ;
		}

		currow++ ;
	}
	if ( currow == endloc )
	{
		currow = p ;
		return ( 1 ) ;
	}
	currow++ ;
	logr++ ;
	curr++ ;

	/* if vertical scrolling is required */
	if ( logr >= 22 )
	{
		logr = 21 ;
		scrollup ( 2, 1, 22, 78 ) ;
		displayline ( currow, 22 ) ;

		/* position `curscr' pointer at the beginning of current screen */
		while ( *curscr != '\n' )
			curscr++ ;
		curscr++ ;
	}

	/* position cursor in appropriate column */
	gotocol() ;

	/* if current cursor row and column is to be displayed */
	if ( display )
	{
		writecol() ;
		writerow() ;
	}

	return ( 0 ) ;
}

/* scrolls the screen contents down */
scrolldown ( int sr, int sc, int er, int ec )
{

	union REGS ii, oo ;

	ii.h.ah = 7 ;  /* service number */
	ii.h.al = 1 ;  /* number of lines to scroll */
	ii.h.ch = sr ;  /* starting row */
	ii.h.cl = sc ;  /* starting column */
	ii.h.dh = er ;  /* ending row */
	ii.h.dl = ec ;  /* ending column */
	ii.h.bh = 27 ;  /* display attribute of blank line created at top */
	int86 ( 16, &ii, &oo ) ;  /* issue interrupt */
}

/* scrolls the screen contents up */
scrollup ( int sr, int sc, int er, int ec )
{
	union REGS ii, oo ;

	ii.h.ah = 6 ;  /* service number */
	ii.h.al = 1 ;  /* number of lines to scroll */
	ii.h.ch = sr ;  /* starting row */
	ii.h.cl = sc ;  /* starting column */
	ii.h.dh = er ;  /* ending row */
	ii.h.dl = ec ;  /* ending column */
	ii.h.bh = 27 ;  /* display attribute of blank line created at bottom */
	int86 ( 16, &ii, &oo ) ;  /* issue interrupt */
}

/* positions cursor in appropriate column */
gotocol()
{
	char *temp ;
	int col ;

	/* increment `temp' to point to character at current cursor location */
	temp = currow ;
	for ( col = 1 ; col < curc ; col++ )
	{
		if ( *temp == '\t' )
			col += 7 ;

		if ( *temp == '\n' )
			break ;

		temp++ ;
	}

	/* if the character at current cursor location is a tab */
	if ( col > curc )
	{
		/* go to the end of tab */
		logc += ( col - curc ) ;
		curc = col ;

		/* if screen needs to be scrolled horizontally */
		if ( curc > 78 )
		{
			logc = 78 ;
			skip = curc - 78 ;
			menubox ( 2, 1, 22, 78, 27, NO_SHADOW ) ;
			displayscreen ( curscr ) ;
		}
	}
}
