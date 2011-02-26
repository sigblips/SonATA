/*******************************************************************************

 File:    systemStatusToCurses.cpp
 Project: OpenSonATA
 Authors: The OpenSonATA code is the result of many programmers
          over many years

 Copyright 2011 The SETI Institute

 OpenSonATA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 OpenSonATA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
 
 Implementers of this code are requested to include the caption
 "Licensed through SETI" with a link to setiQuest.org.
 
 For alternate licensing arrangements, please contact
 The SETI Institute at www.seti.org or setiquest.org. 

*******************************************************************************/

/* 
 * Takes the system status log sse-system-status.txt and creates a
 * curses screen out of it.
 */

#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>


#define BUFF_SIZE 32768

int mRows = 0;
int mCols = 0;
int mNewRows = 0;
int mNewCols = 0;
int mCurrentActivity = -1;

typedef struct _details_info
{
	char line[512];
	struct _details_info *prev;
	struct _details_info *next;
} DetailsInfo;

typedef struct _signal_info
{
	int activityId;
	char name[32];
	DetailsInfo lines;
	struct _signal_info *prev;
	struct _signal_info *next;
} SignalInfo;

int getDetailsInfoCount(DetailsInfo *info);
DetailsInfo *match(DetailsInfo info, char *line);
static void finish(int sig);
DetailsInfo *getAllInCategory(DetailsInfo info, char *category);
void copy(DetailsInfo *toDetailsInfo, DetailsInfo *fromDetailsInfo);
void removeDetailsInfo(DetailsInfo *info, char *line);
void drawSquare(int y, int x, int color);
void ddd(const char *fmt, ...);
void drawBottomMenu();
void freeSigInfo(SignalInfo *info);
void addSigLine(SignalInfo *info, char *line);
void addSigInfo(SignalInfo *info, SignalInfo tempSigInfo);
void addDetailsInfo(DetailsInfo *info, char *line, bool removeDuplicates);
int getSigInfoInfoCount(SignalInfo *info);

int  mGroup             = 0;
bool mGroupChanged      = false;
int  mMaxGroups         = 1;
char mDate[16];
bool mScreenResizeEvent = false;
bool mDemoMode          = false;


// Define the various modes
#define SCREEN_MODE_FIRST      0  //Define this to make the first screen
#define SCREEN_MODE_DETAILS    0
#define SCREEN_MODE_SIG        1
#define SCREEN_MODE_ERROR      2
#define SCREEN_MODE_SUMMARY    3
#define SCREEN_MODE_LAST       3  //Define this to make the last screen


#define LEFT -1
#define RIGHT 1

// Define the top and bottom area sizes if in Overview mode
#define OVERVIEW_TOP_LINES    4
#define OVERVIEW_BOTTOM_LINES 2

//All screens will have a menu at the bottom. Define the number of lines for the menu
#define MENU_LINES 1

int mScreenMode = SCREEN_MODE_DETAILS;

////////////////////////////////////////////////////////
// Catch the control-c to quit screen mode then exit.
////////////////////////////////////////////////////////
static void finish(int sig)
{
	endwin();
	/* do your non-curses wrapup here */
	exit(0);
}

////////////////////////////////////////////////////////
// Catch the xtern screen size change.
////////////////////////////////////////////////////////
static void screenResize(int sig)
{
	mScreenResizeEvent = true;
	FILE *fp = popen("resize", "r");
	if(fp)
	{
		char line[64];
		memset(line, 0, 64);
		while(fgets(line, sizeof(line)-1, fp))
		{
			if(strstr(line, "COLUMNS"))
			{
				mNewCols = atol(line + 16);
			}
			else if(strstr(line, "LINES"))
			{
				mNewRows = atol(line + 14);
			}   
		}
		fclose(fp);
	}
	//ddd("RESIZE %d %d\n\n", mNewCols, mNewRows);
}

////////////////////////////////////////////////////////
// Print debug info to /tmp/screeDebug.txt
////////////////////////////////////////////////////////
void ddd(const char *fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);

	FILE *fp = fopen("/tmp/screeDebug.txt", "a");
	if(!fp)
	{
		va_end(ap);
		return;
	}

	vfprintf(fp, fmt, ap);
	fprintf(fp, "\n");

	fclose(fp);

	va_end(ap);

	return;
}

////////////////////////////////////////////////////////
// Initialize the curses screen
////////////////////////////////////////////////////////
void initScreen()
{

	(void) signal(SIGINT, finish);         /* arrange interrupts to terminate */
	(void) signal(SIGWINCH, screenResize); /* Catch the resize signal */

	initscr();      /* initialize the curses library */
	keypad(stdscr, TRUE);  /* enable keyboard mapping */
	(void) nonl();         /* tell curses not to do NL->CR/NL on output */
	(void) cbreak();       /* take input chars one at a time, no wait for \n */
	(void) noecho();
	timeout(1);

	if (has_colors())
	{
		start_color();

		/*
		 * Simple color assignment, often all we need.  Color pair 0 cannot
		 * be redefined.  This example uses the same value for the color
		 * pair as for the foreground color, though of course that is not
		 * necessary:
		 */
		init_pair(1, COLOR_RED,     COLOR_BLACK);
		init_pair(2, COLOR_GREEN,   COLOR_BLACK);
		init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
		init_pair(4, COLOR_BLUE,    COLOR_BLACK);
		init_pair(5, COLOR_CYAN,    COLOR_BLACK);
		init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(7, COLOR_WHITE,   COLOR_BLACK);
		init_pair(8, COLOR_RED,   COLOR_RED);
		init_pair(9, COLOR_CYAN,   COLOR_CYAN);
		init_pair(10, COLOR_GREEN,   COLOR_GREEN);
		init_pair(11, COLOR_BLACK,   COLOR_BLACK);
		init_pair(12, COLOR_BLACK,   COLOR_YELLOW);
	}

}

////////////////////////////////////////////////////////
// Get the size of a file
////////////////////////////////////////////////////////
int getFileSize(int filedes)
{
	int filesize = 0;

	struct stat buf;
	if (fstat(filedes, &buf) == 0)
	{
		filesize = buf.st_size;
	}
	else 
	{
		fprintf(stderr, "error finding filesize for filedes %d\n",
				filedes);
	}

	return filesize;
}

/**
 * Set the next screen mode based on a left or right arrow button
 * selected by the user.
 **/
void setNextScreenMode(int dir)
{
	if(dir < LEFT)
	{
		if(mScreenMode == SCREEN_MODE_FIRST) return;
		else mScreenMode--;
	}
	else if(dir > RIGHT)
	{
		if(mScreenMode == SCREEN_MODE_LAST) return;
		else mScreenMode++;
	}
}

////////////////////////////////////////////////////////
// Free all memory in an DetailsInfo structure
////////////////////////////////////////////////////////
void freeDetailsInfo(DetailsInfo *info)
{
	DetailsInfo *i = info->next;
	DetailsInfo *j = NULL;
	DetailsInfo *prev = info;
	while(i)
	{
		j = i->next;
		prev->next =  NULL;
		free(i);

		i = j;
	}
}

////////////////////////////////////////////////////////
// Free all memory in an DetailsInfo structure
////////////////////////////////////////////////////////
void freeSigInfo(SignalInfo *info)
{

	SignalInfo *i = info->next;
	SignalInfo *j = NULL;
	SignalInfo *prev = info;
	while(i)
	{
		j = i->next;
		prev->next =  NULL;
		freeDetailsInfo(&i->lines);
		free(i);

		i = j;
	}

}

void addSigLine(SignalInfo *info, char *line)
{
    addDetailsInfo(&info->lines, line, false);

}
void addSigInfo(SignalInfo *info, SignalInfo tempSigInfo)
{
	SignalInfo *i = info->next;
	SignalInfo *prev = info;
	while(i)
	{
		prev = i;
		i = i->next;
	}

	prev->next = (SignalInfo *)calloc(1, sizeof(SignalInfo));
	i = prev->next;

	i->next = NULL;

	copy(&i->lines, &tempSigInfo.lines);

	//Now we need to assign the activity id and name to this entry
	//JJJ
    DetailsInfo *activityInfo = match(tempSigInfo.lines, (char *)"# Activity Id  :");
	if(activityInfo)
	{
		i->activityId = atol(activityInfo->line+strlen("# Activity Id  :") + 1);
	}
    DetailsInfo *nameInfo = match(tempSigInfo.lines, (char *)"# Dx Name     :");
	if(nameInfo)
	{
		strcpy(i->name, nameInfo->line+strlen("# Dx Name     :") + 1);
	}

}

void printSigInfo(SignalInfo *info, int x, int y)
{
	ddd("%d, %s", info->activityId, info->name);
}

void printAllSigReports(SignalInfo info)
{
	SignalInfo *i = info.next;
	while(i)
	{
		printSigInfo(i, 0,0);
		i = i->next;
	}

}
////////////////////////////////////////////////////////
// Remove an entry in an DetailsInfo structure
////////////////////////////////////////////////////////
void removeDetailsInfo(DetailsInfo *info, char *line)
{
	DetailsInfo *i = info->next;
	DetailsInfo *j = NULL;
	DetailsInfo *prev = info;
	while(i)
	{
		if(match(*info, line))
		{
			j = i->next;
			prev->next =  j;
			free(i);

			i = j;
			return;
		}
		else
		{
			i = i->next;
		}
	}
}

////////////////////////////////////////////////////////
// Add an entry int an DetailsInfo structure
////////////////////////////////////////////////////////
void addDetailsInfo(DetailsInfo *info, char *line, bool removeDuplicates)
{
	if(removeDuplicates) removeDetailsInfo(info, line);

	DetailsInfo *i = info->next;
	DetailsInfo *prev = info;
	while(i)
	{
		prev = i;
		i = i->next;
	}

	prev->next = (DetailsInfo *)calloc(1, sizeof(DetailsInfo));
	i = prev->next;

	i->next = NULL;
	strcpy(i->line, line);
}


////////////////////////////////////////////////////////
// Search for a match in an DetailsInfo structure
////////////////////////////////////////////////////////
DetailsInfo *match(DetailsInfo info, char *line)
{
	DetailsInfo *i = info.next;
	if(i == NULL) return NULL;

	//char *p = strtok(line, " \t");
	//if(p == NULL) return NULL;
	int len = (int)strlen(line);

	while(i)
	{
		if(!strncmp(i->line, line, len))
		{
			return i;
		}
		i = i->next;
	}
	return NULL;

}

int contains(DetailsInfo *info, const char *line)
{
	int count = 0;

	DetailsInfo *i = info->next;
	if(i == NULL) return 0;

	while(i)
	{
		if(strstr(i->line, line))
		{
			count++;
		}
		i = i->next;
	}
	return count;

}

int getDetailsInfoCount(DetailsInfo *info)
{
	if(info == NULL) return 0;

	int count = 0;

	DetailsInfo *i = info->next;
	while(i)
	{
		i = i->next;
		count++;
	}
	return count;
}

int getSigInfoInfoCount(SignalInfo *info)
{
	if(info == NULL) return 0;

	int count = 0;

	SignalInfo *i = info->next;
	while(i)
	{
		i = i->next;
		count++;
	}
	return count;
}



bool isInGroup(int y)
{
	if(y < OVERVIEW_TOP_LINES) return true;

	int groupSize = mRows - (OVERVIEW_BOTTOM_LINES + MENU_LINES) - OVERVIEW_TOP_LINES;
	int groupStart = mGroup * groupSize+OVERVIEW_TOP_LINES;
	int groupEnd = groupStart + groupSize - 1;

	//if(mGroup == 0) groupEnd += OVERVIEW_TOP_LINES;

	//0 = 4, 7
	//1 = 12, 19
	//2 = 20, 27

	if(y >= groupStart && y <= groupEnd) return true;
	return false;
}

int posInGroup(int y)
{
	if(y < OVERVIEW_TOP_LINES) return y;

	return  (y-OVERVIEW_TOP_LINES) % (mRows-(OVERVIEW_BOTTOM_LINES+MENU_LINES)-OVERVIEW_TOP_LINES) + OVERVIEW_TOP_LINES;
}

void processCategory(DetailsInfo *screenDetailsInfo, DetailsInfo *newDetailsInfo, 
		DetailsInfo *newScreenDetailsInfo, const char *category, int *y,
		bool *cleared)
{
	//Process arch1, arch2, etc....
	DetailsInfo *sDetailsInfo = getAllInCategory(*screenDetailsInfo, (char *)category);
	DetailsInfo *nDetailsInfo = getAllInCategory(*newDetailsInfo, (char *)category);
	int sCount = getDetailsInfoCount(sDetailsInfo);
	int nCount = getDetailsInfoCount(nDetailsInfo);

	if(sCount != nCount)
	{
		if(isInGroup(*y))
		{
			move(posInGroup(*y), 0);

			clrtobot();
			*cleared = true;
		}
	}
	else
	{
		if(isInGroup(*y))
		{
			move(posInGroup(*y), 0);
		}
	}

	if(nDetailsInfo != NULL)
	{
		DetailsInfo *i = nDetailsInfo->next;
		while(i)
		{
			if(isInGroup(*y))
			{
				if(*cleared == false || mGroupChanged == true)
				{
					//Only draw if not matched, or all need to be redrawn (mGroupChanged == true)
					DetailsInfo *j = match(*screenDetailsInfo, i->line);
					if(j == NULL || strcmp(i->line, j->line) || mGroupChanged == true) //If not match, redraw
					{
						clrtoeol();

						char *p = i->line;

						if(!strcmp(category, "NSS"))
						{
							p += (int)strlen("NSS System Status: ");
						}

						addstr(p);

						if(!strcmp(category, "chan"))
						{
							if(strstr(i->line, "Offline"))
								drawSquare(posInGroup(*y), 9, 8);
							else if(strstr(i->line, "Run"))
								drawSquare(posInGroup(*y), 9, 10);
							else
								drawSquare(posInGroup(*y), 9, 9);
						}
						if(!strcmp(category, "dx"))
						{
							if(strstr(i->line, "Offline"))
								drawSquare(posInGroup(*y), 9, 8);
							else if(strstr(i->line, "Data Coll"))
								drawSquare(posInGroup(*y), 9, 10);
							else
								drawSquare(posInGroup(*y), 9, 9);
						}
						if(!strcmp(category, "arch"))
						{
							if(strstr(i->line, "Connected Dxs: 0"))
								drawSquare(posInGroup(*y), 9, 8);
							else if(strstr(i->line, "Connected Dxs:"))
								drawSquare(posInGroup(*y), 9, 10);
							else
								drawSquare(posInGroup(*y), 9, 9);
						}
						if(!strcmp(category, "ifc"))
						{
							if(strstr(i->line, "Offline"))
								drawSquare(posInGroup(*y), 9, 8);
							else if(strstr(i->line, "OK"))
								drawSquare(posInGroup(*y), 9, 10);
							else
								drawSquare(posInGroup(*y), 9, 9);
						}
					}
				}
				else
				{
					char *p = i->line;

					if(!strcmp(category, "NSS"))
					{
						p += (int)strlen("NSS System Status: ");
					}

					addstr(p);
					if(!strcmp(category, "chan"))
					{
						if(strstr(i->line, "Offline"))
							drawSquare(posInGroup(*y), 9, 8);
						else if(strstr(i->line, "Run"))
							drawSquare(posInGroup(*y), 9, 10);
						else
							drawSquare(posInGroup(*y), 9, 9);
					}
					if(!strcmp(category, "dx"))
					{
						if(strstr(i->line, "Offline"))
							drawSquare(posInGroup(*y), 9, 8);
						else if(strstr(i->line, "Data Coll"))
							drawSquare(posInGroup(*y), 9, 10);
						else
							drawSquare(posInGroup(*y), 9, 9);
					}
					if(!strcmp(category, "arch"))
					{
						if(strstr(i->line, "Connected Dxs: 0"))
							drawSquare(posInGroup(*y), 9, 8);
						else if(strstr(i->line, "Connected Dxs:"))
							drawSquare(posInGroup(*y), 9, 10);
						else
							drawSquare(posInGroup(*y), 9, 9);
					}
					if(!strcmp(category, "ifc"))
					{
						if(strstr(i->line, "Offline"))
							drawSquare(posInGroup(*y), 9, 8);
						else if(strstr(i->line, "OK"))
							drawSquare(posInGroup(*y), 9, 10);
						else
							drawSquare(posInGroup(*y), 9, 9);
					}
				}
			}

			*y = *y + 1;

			if(isInGroup(*y))
			{
				move(posInGroup(*y), 0);
			}

			addDetailsInfo(newScreenDetailsInfo, i->line, true);
			i = i->next;
		}
	}

	if(sDetailsInfo != NULL)
	{
		freeDetailsInfo(sDetailsInfo);
		free(sDetailsInfo);
	}
	if(nDetailsInfo != NULL)
	{
		freeDetailsInfo(nDetailsInfo);
		free(nDetailsInfo);
	}

}

void drawBottomMenu()
{

	int centerPos = 0;
	int numScreenModes = (SCREEN_MODE_LAST - SCREEN_MODE_FIRST + 1);
	int width = (mCols/(numScreenModes+1));

	for(int i = SCREEN_MODE_FIRST; i<=SCREEN_MODE_LAST; i++)
	{
		centerPos = (i+1)*width;
		if(mScreenMode == i) attrset(COLOR_PAIR(6)); //magenta
		else attrset(COLOR_PAIR(5)); //cyan

		if(i == SCREEN_MODE_DETAILS)
		{
			move(mRows-1, centerPos - (int)strlen("F1 - Status")/2);
			addstr("F1 - Status");
		}
		if(i == SCREEN_MODE_SIG)
		{
			move(mRows-1, centerPos - (int)strlen("F2 - Signals")/2);
			addstr("F2 - Signals");
		}
		if(i == SCREEN_MODE_ERROR)
		{
			move(mRows-1, centerPos - (int)strlen("F2 - Errors")/2);
			addstr("F3 - Errors");
		}
		if(i == SCREEN_MODE_SUMMARY)
		{
			move(mRows-1, centerPos - (int)strlen("F4 - Summary")/2);
			addstr("F4 - Summary");
		}

		attrset(COLOR_PAIR(0));
	}
}


void getChanStats(DetailsInfo *newDetailsInfo, char *screenText)
{
	DetailsInfo *nDetailsInfo = getAllInCategory(*newDetailsInfo, (char *)"chan");
	if(nDetailsInfo == NULL)
	{
		sprintf(screenText, "No Channelizers defined");
		return;
	}

	int total = getDetailsInfoCount(nDetailsInfo);
	sprintf(screenText, "Total Channelizers=%d", total);

	int count = contains(nDetailsInfo, "Run");
	if(count > 0)
		sprintf(screenText+strlen(screenText), ", Running=%d", count);

	if(nDetailsInfo != NULL)
	{
		freeDetailsInfo(nDetailsInfo);
		free(nDetailsInfo);
	}
}
void getDxStats(DetailsInfo *newDetailsInfo, char *screenText)
{
	DetailsInfo *nDetailsInfo = getAllInCategory(*newDetailsInfo, (char *)"dx");
	if(nDetailsInfo == NULL)
	{
		sprintf(screenText, "No Dxs defined");
		return;
	}

	int total = getDetailsInfoCount(nDetailsInfo);
	sprintf(screenText, "Total Dxs=%d", total);

	int count = contains(nDetailsInfo, "ffline");
	if(count > 0)
		sprintf(screenText+strlen(screenText), ", Offline=%d", count);
	count = contains(nDetailsInfo, "No Activities");
	if(count > 0)
		sprintf(screenText+strlen(screenText), ", Idle=%d", count);
	count = contains(nDetailsInfo, "Base Accum");
	if(count > 0)
		sprintf(screenText+strlen(screenText), ", Base Accum=%d", count);
	count = contains(nDetailsInfo, "Data Coll");
	if(count > 0)
		sprintf(screenText+strlen(screenText), ", Data Coll=%d", count);
	count = contains(nDetailsInfo, "Sig Det");
	if(count > 0)
		sprintf(screenText+strlen(screenText), ", Sig Det=%d", count);

	float min=10000000.0;
	float max = -min;

	DetailsInfo *i = nDetailsInfo->next;
	while(i)
	{
		char *p = strstr(i->line, "Sky:");
		if(p)
		{
			float num = (float)atof(p+4);
			if(num < min && num != -1.0) min = num;
			else if(num > max && num != -1.0) max = num;
		}
		i = i->next;
	}

	if(min > 0 && max > 0 && max != min)
	{
		char freqText[64];
		sprintf(freqText, "%0.3f to %0.3f MHz", min, max);
		for(int i = (int)strlen(screenText); i < mCols - (int)strlen(freqText); i++)
			strcat(screenText, " ");
		strcat(screenText, freqText);
	}

	if(nDetailsInfo != NULL)
	{
		freeDetailsInfo(nDetailsInfo);
		free(nDetailsInfo);
	}
}

void drawSquare(int y, int x, int color)
{
	move(y, x-1);
	attrset(COLOR_PAIR(color));
	addstr("    ");
	attrset(COLOR_PAIR(0));
}

void drawSummaryScreen(DetailsInfo *screenDetailsInfo, DetailsInfo *newDetailsInfo)
{
	int y = 0;
	bool cleared = false;
	DetailsInfo *newScreenDetailsInfo;
	newScreenDetailsInfo = (DetailsInfo *)calloc(1, sizeof(DetailsInfo));

	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "NSS", &y, &cleared);
	y++;
	move(y, 0);
	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "Component", &y, &cleared);
	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "----", &y, &cleared);

	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "arch", &y, &cleared);
	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "chan", &y, &cleared);
	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "dx", &y, &cleared);
	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "ifc", &y, &cleared);
	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "tscope", &y, &cleared);
	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "array", &y, &cleared);
	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "primary", &y, &cleared);
	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "beam", &y, &cleared);
	processCategory(screenDetailsInfo, newDetailsInfo, newScreenDetailsInfo, "control", &y, &cleared);
	mGroupChanged = false;

	mMaxGroups = ((y-OVERVIEW_TOP_LINES)/(mRows-(OVERVIEW_BOTTOM_LINES+MENU_LINES)-OVERVIEW_TOP_LINES))+1;

	copy(screenDetailsInfo, newScreenDetailsInfo);

	//Create the top line
	int len = (int)strlen(" NSS System Status ");
	move(1, mCols/2 - len/2);
	attrset(COLOR_PAIR(12));
	addstr(" NSS System Status ");
	attrset(COLOR_PAIR(0));
	move(0, mCols/2 - len/2 - 2);
	addch(ACS_ULCORNER);
	move(2, mCols/2 - len/2 - 2);
	addch(ACS_LLCORNER);
	move(0, mCols/2 + len/2 + 2);
	addch(ACS_URCORNER);
	move(2, mCols/2 + len/2 + 2);
	addch(ACS_LRCORNER);
	move(0, mCols/2 - len/2 - 1);
	for(int i = 0; i<=len+1; i++)
		addch(ACS_HLINE);
	move(2, mCols/2 - len/2 - 1);
	for(int i = 0; i<=len+1; i++)
		addch(ACS_HLINE);
	move(1, mCols/2 - len/2 - 2);
	addch(ACS_VLINE);
	move(1, mCols/2 + len/2 + 2);
	addch(ACS_VLINE);

	attrset(COLOR_PAIR(3));
	char screenText[mCols+1];
	sprintf(screenText, "Screen %d of %d", mGroup+1, mMaxGroups);
	move(0, mCols - strlen(screenText)-1);
	addstr(screenText);
	if((mGroup+1) == mMaxGroups)
	{
		move(1, mCols - strlen(screenText)/2-1 - 3);
		addstr("(pg up)");
	}
	else if((mGroup) == 0)
	{
		move(1, mCols - strlen(screenText)/2-1 - 3);
		addstr("(pg dn)");
	}
	else
	{
		move(1, mCols - strlen(screenText)/2-1 - 5);
		addstr("(pg up/dn)");
	}

	sprintf(screenText, "Current Activity: %d", mCurrentActivity);
	move(1,0);
	addstr(screenText);

	//Create the bottom lines
	move(mRows - 1 - MENU_LINES, 0);
	clrtoeol();
	getDxStats(newDetailsInfo, screenText);
	addstr(screenText);

	move(mRows - 2 - MENU_LINES, 0);
	clrtoeol();
	getChanStats(newDetailsInfo, screenText);
	addstr(screenText);

	attrset(COLOR_PAIR(0));

	drawBottomMenu();

	//FREE!
	freeDetailsInfo(newScreenDetailsInfo);
	free(newScreenDetailsInfo);
}

DetailsInfo *getAllInCategory(DetailsInfo info, char *category)
{
	DetailsInfo *returnDetailsInfo = NULL;
	DetailsInfo *i = info.next;
	int len = (int)strlen(category);
	while(i)
	{
		if(!strncmp(i->line, category, len))
		{
			if(returnDetailsInfo == NULL)
			{
				returnDetailsInfo = (DetailsInfo *)calloc(1, sizeof(DetailsInfo));
			}
			addDetailsInfo(returnDetailsInfo, i->line, true);

		}
		i = i->next;
	}

	return returnDetailsInfo;
}

void copy(DetailsInfo *toDetailsInfo, DetailsInfo *fromDetailsInfo)
{
	freeDetailsInfo(toDetailsInfo);

	DetailsInfo *i = fromDetailsInfo->next;

	while(i)
	{
		addDetailsInfo(toDetailsInfo, i->line, true);
		i = i->next;
	}
}

void strip(char *line)
{
	if(line == NULL) return;

	for(int i=0; i<(int)strlen(line); i++)
		if(line[i] == 10 || line[i] == 13) line[i] = 0;

	//Strip out UTC
	char *p = strstr(line, "UTC  ");
	while(p)
	{
		strcpy(p, p+5);
		p = strstr(line, "UTC");
	}



	//Strip out date
	if(strstr(line, "NSS") == NULL && mDate[0] != 0)
	{
		p = strstr(line, mDate);
		while(p)
		{
			strcpy(p, p+strlen(mDate)+1);
			p = strstr(line, mDate);
		}
	}

	//Strip out the default date
	if(strstr(line, "NSS") == NULL)
	{
		p = strstr(line, "1970-01-01 ");
		while(p)
		{
			strcpy(p, p+strlen("1970-01-01")+1);
			p = strstr(line, "1970-01-01");
		}
	}

	/* Do later
	   if(!strncmp(line, "array\t", strlen("array\t")))
	   strcpy(line+strlen("array\t"), line+strlen("array \t\t"));
	   */
	p = strstr(line, "\t\t");
	if(p)
	{
		char temp[256];
		memset(temp, 0, sizeof(temp));
		strcpy(temp, line+(int)(p - line)+2);
		line[(int)(p - line)] = 0;
		strcat(line, "       ");
		if(strstr(line, "array")) strcat(line, "  ");
		strcat(line, temp);
	}



}

int main(int argc, char *argv[])
{
	DetailsInfo screenDetailsInfo;
	DetailsInfo newDetailsInfo;
	DetailsInfo tempDetailsInfo;
	SignalInfo  sigInfo;
	SignalInfo  tempSigInfo;
	char ch = 0;
	bool inSigReport = false;
	memset(mDate, 0, sizeof(mDate));

	char line[1024];
	char commandStatusLog[1024];
	char commandSysLog[1024];

	if (argc < 1)
	{
		fprintf(stderr, "usage: systemStatusToCurses <~/sonata_archive/templogs/sse-system-status.txtn");
		exit(EXIT_FAILURE);
	}

	mCols = 120;
	mRows = 20;
	memset(commandStatusLog, 0, sizeof(commandStatusLog));
	memset(commandSysLog, 0, sizeof(commandSysLog));

	for(int i = 0; i< argc - 1; i++)
	{
		if(!strncmp(argv[i], "-statuslog", 10))
		{
			if(mDemoMode == false)
				sprintf(commandStatusLog, "tail -f %s", argv[i+1]);
			else
				sprintf(commandStatusLog, "cat %s", argv[i+1]);
		}
		else if(!strncmp(argv[i], "-syslog", 7))
		{
			if(mDemoMode == false)
				sprintf(commandSysLog, "tail -f %s", argv[i+1]);
			else
				sprintf(commandSysLog, "cat %s", argv[i+1]);
		}
		else if(!strncmp(argv[i], "-demo", 5))
		{
			mDemoMode = true;
		}
	}

	//Call screenResize(0 which gets the size of the screen
	screenResize(0);

	//Initialize info
	screenDetailsInfo.next = NULL;
	newDetailsInfo.next    = NULL;
	tempDetailsInfo.next   = NULL;
	sigInfo.next           = NULL;
	sigInfo.lines.next     = NULL;
	tempSigInfo.next       = NULL;
	tempSigInfo.lines.next = NULL;

	//Initialize the curses screen
	initScreen();

	FILE *statusFp = popen(commandStatusLog, "r");
	if(statusFp == NULL)
	{
		fprintf(stdout," Could not open %s, EXITING.\n", commandStatusLog);
	}

	FILE *systemFp = popen(commandSysLog, "r");
	if(systemFp == NULL)
	{
		fclose(statusFp);
		fprintf(stdout," Could not open %s, EXITING.\n", commandSysLog);
	}

	fd_set rfds;
	struct timeval tv;
	int retval = 0;

	int statusFd = fileno(statusFp);
	int systemFd = fileno(systemFp);

	while(1)
	{
		// If there was a screen size change, get the new screen
		// size, force a redraw.
		if(mScreenResizeEvent == true)
		{

			if(mNewRows != mRows || mNewCols != mCols)
			{
				mRows = mNewRows;
				mCols = mNewCols;
				endwin();
				initScreen();
				refresh();

				if(mScreenMode == SCREEN_MODE_DETAILS)
				{
					mGroupChanged = true;
					move(0, 0);
					clrtobot();
					copy(&tempDetailsInfo, &newDetailsInfo);
					copy(&newDetailsInfo, &screenDetailsInfo);
					drawSummaryScreen(&screenDetailsInfo, &newDetailsInfo);
					copy(&newDetailsInfo, &tempDetailsInfo);
					freeDetailsInfo(&tempDetailsInfo);
				}

				mScreenResizeEvent = false;
			}
		}

		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		FD_SET(statusFd, &rfds);
		FD_SET(systemFd, &rfds);

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		int maxFd = statusFd;
		if(systemFd > statusFd) maxFd = systemFd;
		retval = select(maxFd+1, &rfds, NULL, NULL, &tv);

		//if(retval == -1) finish(0);

		//User input key presses
		if(FD_ISSET(0, &rfds))
		{

			ch = getch();

			if(ch == 9) //F1
			{
				mScreenMode = SCREEN_MODE_DETAILS;
				erase();
				drawBottomMenu();
				mGroupChanged = true;
			}
			if(ch == 10) //F2
			{
				mScreenMode = SCREEN_MODE_SIG;
				erase();
				drawBottomMenu();
			}
			if(ch == 11) //F3
			{
				mScreenMode = SCREEN_MODE_ERROR;
				erase();
				drawBottomMenu();
			}
			if(ch == 12) //F4
			{
				mScreenMode = SCREEN_MODE_SUMMARY;
				erase();
				drawBottomMenu();
			}

			//Up or down if in OVERVIEW mode
			if(mScreenMode == SCREEN_MODE_DETAILS)
			{

				if(ch == 83) //Page Up
				{
					if(mGroup > 0)
					{
						mGroupChanged = true;
						move(0, 0);
						clrtobot();
						mGroup --;
						copy(&tempDetailsInfo, &newDetailsInfo);
						copy(&newDetailsInfo, &screenDetailsInfo);
						drawSummaryScreen(&screenDetailsInfo, &newDetailsInfo);
						copy(&newDetailsInfo, &tempDetailsInfo);
						freeDetailsInfo(&tempDetailsInfo);
					}
					if(mGroup < 0) mGroup = 0;
				}
				else if(ch == 82) //Page Down
				{
					if(mGroup < mMaxGroups-1)
					{
						mGroupChanged = true;
						move(0, 0);
						clrtobot();
						mGroup ++;
						copy(&tempDetailsInfo, &newDetailsInfo);
						copy(&newDetailsInfo, &screenDetailsInfo);
						drawSummaryScreen(&screenDetailsInfo, &newDetailsInfo);
						copy(&newDetailsInfo, &tempDetailsInfo);
						freeDetailsInfo(&tempDetailsInfo);
					}
				}
			}
		}

		//The status log
		if(FD_ISSET(statusFd, &rfds))
		{
			fgets(line, sizeof(line)-1, statusFp);
			if(strlen(line)>=200) ddd("1, %s", line);

			strip(line);
			line[mCols-1] = 0;
			addDetailsInfo(&newDetailsInfo, line, true);

			//The end of the data is marked by "NSS..."
			if(!strncmp(line, "NSS", 3)) 
			{
				//Get the date, the 4th fiels of the NSS line
				char *p = strtok(line, " "); 
				if(p) p = strtok(NULL, " "); 
				if(p) p = strtok(NULL, " "); 
				if(p) p = strtok(NULL, " "); 
				if(p && strlen(p) < (sizeof(mDate) - 1)) strcpy(mDate, p);

				if(mScreenMode == SCREEN_MODE_DETAILS)
				{
					drawSummaryScreen(&screenDetailsInfo, &newDetailsInfo);
					copy(&screenDetailsInfo, &newDetailsInfo);
				}
				freeDetailsInfo(&newDetailsInfo);
				refresh();
				if(mDemoMode) usleep(600000);
			}
		}

		//The system log
		if(FD_ISSET(systemFd, &rfds))
		{
			fgets(line, sizeof(line)-1, systemFp);
			strip(line);
			line[mCols-1] = 0;

			if(strlen(line)>=200) ddd("2, %s", line);

			//Save the activity ID
			char *p = strstr(line, " UTC Act ");
			if(p)
			{
				mCurrentActivity = atol(p + 9);
			}

			/*
			//Start of a sig detection report "# Activity Name:"
			if(!strncmp(line, "# Activity Name:", strlen("# Activity Name:")))
			{
				inSigReport = true;
				freeSigInfo(&tempSigInfo);
				addSigLine(&tempSigInfo, line);
				ddd("Start of a report");
			}

			//End of sig report is "#   -----  Page"
			else if(!strncmp(line, "#   -----  Page", strlen("#   -----  Page")))
			{
				inSigReport = false;
				addSigInfo(&sigInfo, tempSigInfo);
                int count = getSigInfoInfoCount(&sigInfo);
				ddd( "END of a report Count=%d", count);
				printAllSigReports(sigInfo);
			}
			else if(inSigReport == true && strlen(line) > 3)
			{
				addSigLine(&tempSigInfo, line);
				ddd("%s", line);

			}
			*/


		}

	}
}