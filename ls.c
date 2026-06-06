#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

struct FileInfo {
  char name[DIRSIZ + 1];
  struct stat st;
};

void
swap(struct FileInfo *a, struct FileInfo *b)
{
  struct FileInfo temp = *a;
  *a = *b;
  *b = temp;
}

void
heapify(struct FileInfo arr[], int n, int i)
{
  int largest = i;
  int left = 2 * i + 1;
  int right = 2 * i + 2;

  if (left < n && arr[left].st.size > arr[largest].st.size)
    largest = left;

  if (right < n && arr[right].st.size > arr[largest].st.size)
    largest = right;

  if (largest != i) {
    swap(&arr[i], &arr[largest]);
    heapify(arr, n, largest);
  }
}

void
heapSort(struct FileInfo arr[], int n)
{
  for (int i = n / 2 - 1; i >= 0; i--)
    heapify(arr, n, i);

  for (int i = n - 1; i > 0; i--) {
    swap(&arr[0], &arr[i]);
    heapify(arr, i, 0);
  }
}

char*
fmtname(char *path, short type)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  //Adding '/' at the end if the type is Directory
  if (type == T_DIR) {
    *(p+strlen(p)) = '/';
    *(p+strlen(p)+1) = '\0';
  }

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path, int needSort)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
      printf(1, "%s %d %d %d\n", fmtname(path, st.type), st.type, st.ino, st.size);
      break;

  case T_DIR:
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf(1, "ls: path too long\n");
        break;
      }

      if (needSort == 0)
      {
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
          if(de.inum == 0)
            continue;

          if (de.name[0] == '.')
            continue;

          memmove(p, de.name, DIRSIZ);
          p[DIRSIZ] = 0;
          if(stat(buf, &st) < 0){
            printf(1, "ls: cannot stat %s\n", buf);
            continue;
          }
          printf(1, "%s %d %d %d\n", fmtname(buf, st.type), st.type, st.ino, st.size);
        }
        break;
      }

      struct FileInfo files[85];
      int fileCount = 0;

      strcpy(buf, path);
      p = buf+strlen(buf);
      *p++ = '/';

      while(read(fd, &de, sizeof(de)) == sizeof(de) && fileCount < 85){
        if(de.inum == 0 || de.name[0] == '.')
          continue;

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
          printf(1, "ls: cannot stat %s\n", buf);
          continue;
        }

        files[fileCount].st.size = st.size;
        files[fileCount].st.ino = st.ino;
        files[fileCount].st.dev = st.dev;
        files[fileCount].st.type = st.type;
        files[fileCount].st.nlink = st.nlink;

        int i;
        for (i = 0; i < DIRSIZ && de.name[i] != '\0'; i++) {
          files[fileCount].name[i] = de.name[i];
        }
        files[fileCount].name[i] = '\0';
        fileCount++;
      }

      heapSort(files, fileCount);

      for (int i = fileCount - 1; i >= 0; i--) {
        printf(1, "%s %d %d %d\n", fmtname(files[i].name, files[i].st.type), files[i].st.type, files[i].st.ino, files[i].st.size);
      }
      break;

  default:
      break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;
  int needSort = 0;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i],"-s") == 0) {
      needSort = 1;
    }
  }

  int path_found = 0;
  for(i = 1; i < argc; i++){
    if(argv[i][0] != '-'){
      ls(argv[i], needSort);
      path_found = 1;
    }
  }

  if (!path_found) {
    ls(".", needSort);
  }
  exit();
}
