#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void)
{
  fs_mkdir(".beargit");

  FILE *findex = fopen(".beargit/.index", "w");
  fclose(findex);

  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");

  return 0;
}

/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char *filename)
{
  FILE *findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while (fgets(line, sizeof(line), findex))
  {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0)
    {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status()
{
  /*     read a string of at most <size> (incl. NULL character) 
       from file <filename> and store it into <str>. 
       Note that <str>needs to be large enough to hold that string.
 */

  //FILENAME_SIZE
  FILE *currIndex = fopen(".beargit/.index", "r");
  char line[FILENAME_SIZE];
  int count = 0;
  fprintf(stdout, "%s\n\n", "Tracked files:");
  /*
  fgets函数功能为从指定的流中读取数据，每次读取一行。
  其原型为：char *fgets(char *str, int n, FILE *stream);
  从指定的流 stream 读取一行，并把它存储在 str 所指向的字符串内。
  当读取 (n-1) 个字符时，或者读取到换行符时，或者到达文件末尾时，它会停止
  */
  while (fgets(line, FILENAME_SIZE, currIndex))
  {

    count += 1;
    fprintf(stdout, "  %s", line);
  }
  fprintf(stdout, "\n%d files total\n", count);
  fclose(currIndex);
  return 0;
}

/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char *filename)
{
  FILE *findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  int isFound = 0;
  char line[FILENAME_SIZE];

  while (fgets(line, sizeof(line), findex))
  {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0)
    {
      isFound = 1;
      continue;
    }
    fprintf(fnewindex, "%s\n", line);
  }
  if (isFound == 0)
  {
    fprintf(stderr, "ERROR: File %s not tracked\n", filename);
    fclose(findex);
    fclose(fnewindex);
    fs_rm(".beargit/.newindex");
    return 3;
  }

  fclose(findex);
  fclose(fnewindex);
  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char *go_bears = "GO BEARS!";

int is_commit_msg_ok(const char *msg)
{
  if (strstr(msg, go_bears))
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

//helper function to cancat string
char *concat(const char *s1, const char *s2)
{
  char *result = malloc(strlen(s1) + strlen(s2) + 1); //+1 for the zero-terminator
  //in real code you would check for errors in malloc here
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

void next_commit_id(char *commit_id)
{
  char *p = commit_id;
  int flag = 0;
  while (*p != '\0')
  {
    switch (*p)
    {
    case '6':
      *p = '1';
      flag = 0;
      break;
    case '1':
      *p = 'c';
      flag = 0;
      break;
    case 'c':
      *p = '6';
      p++;
      flag = 1;
      break;
    default:
      *p = '6';
      p++;
      flag = 1;
      break;
      if (flag = 0)
      {
        break;
      }
    }
  }
}

int beargit_commit(const char *msg)
{
  if (!is_commit_msg_ok(msg))
  {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);
  /* COMPLETE THE REST */

  char *newDir = (char *)malloc(sizeof(".beargit/") + sizeof(commit_id) + 1);
  sprintf(newDir, ".beargit/%s", commit_id);
  fs_mkdir(newDir);

  char *newIndex = (char *)malloc(sizeof(newDir) + sizeof("/.index") + 1);
  sprintf(newIndex, "%s/.index", newDir);
  fs_cp(".beargit/.index", newIndex);
  //free(newIndex);

  char *newPrev = (char *)malloc(sizeof(newDir) + sizeof("/.prev") + 1);
  sprintf(newPrev, "%s/.prev", newDir);
  fs_cp(".beargit/.prev", newPrev);
  //free(newPrev);

  // copy all tracked files to .beagit/<newid>
  FILE *findex = fopen(".beargit/.index", "r");
  char line[FILENAME_SIZE];

  while (fgets(line, FILENAME_SIZE, findex))
  {
    strtok(line, "\n");
    char *newFile = (char *)malloc(sizeof(newDir) + sizeof("/") + strlen(line) + 1);
    sprintf(newFile, "%s/%s", newDir, line);
    fs_cp(line, newFile);
    //free(newFile);
  }

  char *newMsgDir = (char *)malloc(sizeof(newDir) + sizeof("/.msg") + 1);
  sprintf(newMsgDir, "%s/.msg", newDir);
  write_string_to_file(newMsgDir, msg);

  fclose(findex);

  write_string_to_file(".beargit/.prev", commit_id);

  free(newMsgDir);
  free(newDir);
  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */

int beargit_log()
{
  /* COMPLETE THE REST */

  return 0;
}
