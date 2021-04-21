/*
	Name: getProcStats.c
	Copyright: 
	Author: Meraj Hussain syed
	Date: 12/03/15 13:49
	Description: Gives details of the process resource usage
*/


#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>

#define ERROR_OPENDIR 100
#define END_OF_HEAP 9
#define HEAP_ANALYSIS_DONE 14
int pStats[7];
int UStime[2];

int
main ()
{

  int ret;
  char buff[1024];
  int length;
  int index;
  int len, i;
  struct dirent *pDirent;
  DIR *pDir;
  int track_process = 1;
  char dir[100];
  char dir1[100];
  char dirFd[100];
  char dirSmaps[100];
  char buff1[1024];
  char dirSockets[100];
  char pNameStat[100];
  char cpuStatProc[100];
  char avgCpu[100];
  char *chipNewLine;
  int openFds;
  pDir = opendir ("/proc");

  printf
    ("\n Note!! Need root user access to run this application!!!!!!!!!!!!!\n ");


  if (pDir == NULL)
    {
      printf ("Cannot open directory ");
      return 1;
    }




  printf ("\n*********************************************************\n");
  printf ("\n***********Process Resource Usage Diagnosis *************\n");
  printf ("\n*********************************************************\n");
  while ((pDirent = readdir (pDir)) != NULL)
    {

      track_process++;
      if (isProcess (pDirent->d_name) == 1)
        {
          printf ("\nprocess id::%s", pDirent->d_name);
          strcpy (dir1, "/proc/");
          strcpy (avgCpu, dir1);
          strcat (dir1, pDirent->d_name);
          strcpy (dirFd, dir1); /* to find number of file descriptors open */
          strcat (dirFd, "/fd");
          strcpy (dirSmaps, dir1);
          strcat (dirSmaps, "/smaps");
          strcpy (dirSockets, dir1);
          strcat (dirSockets, "/fd");
          strcpy (pNameStat, dir1);
          strcat (pNameStat, "/status");
          strcpy (cpuStatProc, dir1);
          strcat (cpuStatProc, "/stat");
          strcat (avgCpu, "stat");
          strcat (dir1, "/statm");


          showPidAndName (pNameStat);

          printf ("\n\nFile Descriptor Usage Stats ::\n");
          showOpenFds (dirFd);

          printf ("\n\nHeap Usage Usage Stats ::\n");
          showHeapUsage (dirSmaps);

          printf ("\n\nSocket Usage Stats ::\n");
		  showSocketStats (dirSockets);


          printf ("\n\nProcess Memory Stats ::\n");
          showProcessMemStats (dir1);

          /*   showCpuStats(cpuStatProc,avgCpu); */

          printf
            ("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
          memset ((char *) buff, '\0', 1024);
        }
    }


  //sleep(500);

  return 0;
}


int
isProcess (char *procName)
{

  char digit[10] = { '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9'
  };

  int i;
  int j;
  int length = strlen (procName);
  int count = 0;
  int flag = 0;
  for (i = 0; i < length; i++)
    {
      flag = 0;
      for (j = 0; j < 9; j++)
        {
          if (procName[i] == digit[j])
            {
              flag = 1;
              break;
            }
        }
      if (flag == 1)
        {
          count++;
        }


    }


  return ((count == length) ? 1 : 0);
  
}


void
showProcessMemStats (char str[])
{

  int length;
  char *token;
  int i = 0;
  char buff[1024];
  int ret;
  char *chipNewLine;

  ret = open (str, O_RDONLY);
  if (ret == -1)
    {
      perror ("file open");
    }
  memset ((char *) buff, '\0', 1024);

  while (read (ret, buff, 1024) > 0)
    {


    }
  chipNewLine = buff;
  chipNewLine += strlen (buff) - 1;
  *chipNewLine = NULL;


  length = strlen (buff);

  token = strtok (buff, " ");
   while (token != NULL)
    {

      pStats[i++] = atoi (token);
      token = strtok (NULL, " ");

    }



  printf ("\nTotal program Size = %d pages", pStats[0]);
  printf ("\nResident size      = %d pages", pStats[1]);
  printf ("\nShared Pages       = %d pages", pStats[2]);
  printf ("\nText               = %d pages", pStats[3]);
  printf ("\nlib                = %d pages", pStats[4]);
  printf ("\ndata & stack       = %d pages", pStats[5]);
  printf ("\ndirty pages        = %d pages", pStats[6]);

  printf ("\n");
  close (ret);

}

void
showOpenFds (char fd[])
{

  struct dirent *pDirent;
  DIR *pDir;
  int openFdCount = 0;
  pDir = opendir (fd);



  if (pDir == NULL)
    {
      printf ("Cannot open directory ");
      return ERROR_OPENDIR;
    }


  printf ("\nThe Open fds are ");
  while ((pDirent = readdir (pDir)) != NULL)
    {

      if ((strcmp (pDirent->d_name, ".") != 0) &&
          (strcmp (pDirent->d_name, "..") != 0))
        {
          printf (" %s ", pDirent->d_name);
          openFdCount++;
        }

    }


  printf ("\n Total file descriptors open = %d", openFdCount);


}

void
showHeapUsage (char smpasAnalysis[])
{

  char *token;
  int i = 0;
  int ret;
  char buff[102400];
  int heapFound = 0, count = 0;


  ret = open (smpasAnalysis, O_RDONLY);

  memset ((char *) buff, '\0', 102400);

  while (read (ret, buff, 102400) > 0)
    {

      //printf("%s",buff);

    }

  token = strtok (buff, "\n");
  while (token != NULL)
    {
      if (strstr (token, "[heap]"))
        {
          heapFound = 1;
          //printf("\n%s",token);
        }

      if (heapFound == 1 && count < END_OF_HEAP)
        {
          printf ("\n%s", token);
          count++;
        }
      if (count == END_OF_HEAP)
        {
          heapFound = HEAP_ANALYSIS_DONE;
        }
      token = strtok (NULL, "\n");
	  
	  }
 if (heapFound != HEAP_ANALYSIS_DONE)
    {
      printf ("\n Sorry No heap data recorded for this process");
    }

  printf ("\n");
  close (ret);
}

void
showSocketStats (char sockstat[])
{

  int ret;
  FILE *execCommand;
  int openSocketCount = 0;
  char *command;
  char commandOutput[1024];

  command = (char *) malloc (100);
  strcpy (command, "ls -l ");
  strcat (command, sockstat);
  execCommand = popen (command, "r");

  while (fgets (commandOutput, 1024, execCommand) > 0)
    {

      if (strstr (commandOutput, "socket"))
        {

          openSocketCount++;
          printf ("\n%s", commandOutput);
        }
    }

  if (openSocketCount > 0)
    {
      printf ("\ntotal Open Sockets = %d", openSocketCount);
    }

  else
    {
      printf ("\nNo open Sockets");
    }
   free(command);
   pclose(execCommand);
}


void
showPidAndName (char fileName[])
{

  int ret;
  char procStatus[1024];
  char *token;
  int count;
  ret = open (fileName, O_RDONLY);

  memset ((char *) procStatus, '\0', 1024);

  while (read (ret, procStatus, 1024) > 0);

  token = strtok (procStatus, "\n");
  count = 1;

  while (count <= 2)
    {
      printf ("\n%s", token);
      token = strtok (NULL, "\n");
      count++;
    }

  printf ("\n");

  close (ret);

}

unsigned long
readTotCpu (char tCpu[])
{

  int ret;
  char totCpu[1024];
  char *token;
  char *buff;
  int count = 0;
  unsigned long totalCpu = 0;



  ret = open (tCpu, O_RDONLY);

  while (read (ret, totCpu, 1024) > 0);

  buff = strtok (totCpu, "\n");

  token = strtok (buff, " ");
  while (token != NULL)
    {
      if (count > 1)
        {
          totalCpu += atoi (token);
        }
      token = strtok (NULL, " ");
      count++;
    }
  memset ((char *) totCpu, '\0', 1024);

  printf ("\n%lu ", totalCpu);

  close (ret);
  return totalCpu;

}

void
readUSTime (char tCpu[])
{

  int ret;
  char usTime[1024];
  char *token;
  int count = 0;
  int utime;
  int stime;


  unsigned long totalCpu = 0;



  ret = open (tCpu, O_RDONLY);

  while (read (ret, usTime, 1024) > 0);
  token = strtok (usTime, " ");
  while (token != NULL)
    {
      if (count == 13)
        {
          UStime[0] = atoi (token);

        }
      if (count == 14)
        {
          UStime[1] = atoi (token);
        }

      token = strtok (NULL, " ");
      count++;
    }

  close (ret);
}

void
showCpuStats (char file1[], char file2[])
{
  unsigned long totalCpuBefore;
  unsigned long totalCpuAfter;
  int utimeBefore, utimeAfter;
  int stimeBefore, stimeAfter;
  double sys_util, user_util;


  readUSTime (file1);
  utimeBefore = UStime[0];
  stimeBefore = UStime[1];
  totalCpuBefore = readTotCpu (file2);
  sleep (1);
  totalCpuAfter = readTotCpu (file2);
  readUSTime (file1);
  utimeAfter = UStime[0];
  stimeBefore = UStime[1];

  user_util =
    100 * (utimeAfter - utimeBefore) / (totalCpuAfter - totalCpuBefore);
  sys_util =
    100 * (stimeAfter - stimeBefore) / (totalCpuAfter - totalCpuBefore);

  printf ("\nsys_util = %lf user_util = %lf%", sys_util, user_util);

}
