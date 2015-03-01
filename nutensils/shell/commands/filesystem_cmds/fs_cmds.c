/*
 * Copyright (c) 2015 Michael Stuart.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the nutensils project, <https://github.com/drmetal/nutensils>
 *
 * Author: Michael Stuart <spaceorbot@gmail.com>
 *
 */


#include "minstdlib.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "shell.h"
#include "fs_cmds.h"

#define IS_NOT_A_DIRECTORY          " is not a directory"
#define FORMATTING_SDCARD           "formatting sdcard"
#define ARGUMENT_NOT_SPECIFIED      "argument not specified"
#define ERROR_OPENING_SOURCE_FILE      "couldnt open source file"
#define ERROR_OPENING_DEST_FILE      "couldnt open destination file"
#define ERROR_MOVING_FILE            "error moving file"

const char* units[] = {
       "b", "kb", "Mb", "Gb"
};

#define PAD_TO_FILESIZE     40
#define PAD_TO_NEXT_FILE    16

void install_fs_cmds(shellserver_t* sh)
{
    register_command(sh, &sh_ls_cmd, NULL, NULL, NULL);
    register_command(sh, &sh_cd_cmd, NULL, NULL, NULL);
    register_command(sh, &sh_rm_cmd, NULL, NULL, NULL);
    register_command(sh, &sh_mkdir_cmd, NULL, NULL, NULL);
    register_command(sh, &sh_echo_cmd, NULL, NULL, NULL);
    register_command(sh, &sh_cat_cmd, NULL, NULL, NULL);
    register_command(sh, &sh_mv_cmd, NULL, NULL, NULL);
    register_command(sh, &sh_cp_cmd, NULL, NULL, NULL);
}

int sh_ls(int fdes, const char** args, unsigned char nargs)
{
    unsigned int i;
    char size[8];
    struct stat st;
    DIR *dir;
    struct dirent *ent;
    bool ll = has_switch((const char*)"-l", args, nargs);
    const char* rel = final_arg(args, nargs);
    char* buffer = malloc(256);

    if(buffer)
    {
        if(getcwd(buffer, SHELL_CWD_LENGTH_MAX))
        {
            if((nargs == 1 && !ll) || (nargs == 2 && ll))
            {
                strcat(buffer, rel);
            }

            dir = opendir(buffer);
            free(buffer);

            if(dir)
            {
                while(1)
                {
                    ent = readdir(dir);
                    if(ent == NULL)
                        break;

                    if(ent->d_type == DT_DIR)
                        send(fdes, DIR_TEXT_START, sizeof(DIR_TEXT_START)-1, 0);

                    i = strlen(ent->d_name);
                    send(fdes, ent->d_name, i, 0);

                    if(ent->d_type == DT_DIR)
                        send(fdes, DIR_TEXT_STOP, sizeof(DIR_TEXT_STOP)-1, 0);

                    if(ll)
                    {
                        // pad spaces
                        while(i++ < PAD_TO_FILESIZE)
                            send(fdes, " ", 1, 0);

                        if(ent->d_type == DT_REG)
                        {

                            // if its a regular file
                            if(stat(ent->d_name, &st) == 0)
                            {
                                i = 0;
                                while(st.st_size > 1000 && (i < sizeof(units)/sizeof(units[0])))
                                {
                                    i++;
                                    st.st_size /= 1000;
                                }
                                sprintf(size, "%d%s", st.st_size, units[i]);
                                send(fdes, size, strlen(size), 0);
                            }
                        }
                        else
                            send(fdes, "-", 1, 0);

                        send(fdes, SHELL_NEWLINE, sizeof(SHELL_NEWLINE)-1, 0);
                    }
                    else
                    {
                        // pad spaces
                        while(i++ < PAD_TO_NEXT_FILE)
                            send(fdes, " ", 1, 0);
                    }
                }
                closedir(dir);
            }
        }
    }
    return SHELL_CMD_EXIT;
}

int sh_cd(int fdes, const char** args, unsigned char nargs)
{
    const char* path;

    if(!nargs)
        path = "/";
    else
        path = arg_by_index(0, args, nargs);

    if(chdir(path) == -1)
    {
      send(fdes, path, strlen(path), 0);
      send(fdes, IS_NOT_A_DIRECTORY, sizeof(IS_NOT_A_DIRECTORY)-1, 0);
    }
    return SHELL_CMD_EXIT;
}

int sh_rm(int fdes, const char** args, unsigned char nargs)
{
    unsigned char arg = 0;

    if(!nargs)
    {
        send(fdes, ARGUMENT_NOT_SPECIFIED, sizeof(ARGUMENT_NOT_SPECIFIED)-1, 0);
    }

    while(arg < nargs)
    {
        unlink(arg_by_index(arg, args, nargs));
        arg++;
    }

    return SHELL_CMD_EXIT;
}

int sh_mkdir(int fdes, const char** args, unsigned char nargs)
{
    const char* dir = final_arg(args, nargs);

    if(dir)
        mkdir(dir, 0777);
    else
        send(fdes, ARGUMENT_NOT_SPECIFIED, sizeof(ARGUMENT_NOT_SPECIFIED)-1, 0);

    return SHELL_CMD_EXIT;
}

int sh_echo(int fdes, const char** args, unsigned char nargs)
{
    (void)fdes;
    (void)nargs;
    FILE* f;
    const char* string = args[0];
    const char* option = args[1];
    const char* filename = args[2];
    const char* operate;

    if(!strncmp(">>", (const char*)option, 2))
        operate = "a";
    else if(!strncmp(">", (const char*)option, 1))
        operate = "w";
    else
        return SHELL_CMD_PRINT_USAGE;

    f = fopen(filename, operate);
    if(*operate == 'a')
        fputc('\n', f);
    fputs(string, f);
    fclose(f);

    return SHELL_CMD_EXIT;
}

int sh_cat(int fdes, const char** args, unsigned char nargs)
{
	(void)nargs;
	char buffer[64];
	int len;
	int ffd = open(args[0], O_RDONLY);

	if(ffd != -1)
	{
		len = 1;
		while(len > 0)
		{
			len = read(ffd, buffer, sizeof(buffer));
			if(len > 0)
				send(fdes, buffer, len, 0);
		}
		close(ffd);
	}
    return SHELL_CMD_EXIT;
}

int sh_mv(int fdes, const char** args, unsigned char nargs)
{
    const char* path = arg_by_index(0, args, nargs);
    const char* newpath = arg_by_index(1, args, nargs);

    if(path && newpath)
    {
        if(rename(path, newpath) == -1)
            send(fdes, ERROR_MOVING_FILE, sizeof(ERROR_MOVING_FILE)-1, 0);
    }
    else
        send(fdes, ARGUMENT_NOT_SPECIFIED, sizeof(ARGUMENT_NOT_SPECIFIED)-1, 0);

    return SHELL_CMD_EXIT;
}

int sh_cp(int fdes, const char** args, unsigned char nargs)
{
    FILE* f1;
    FILE* f2;
    const char* path = arg_by_index(0, args, nargs);
    const char* newpath = arg_by_index(0, args, nargs);
    char buffer[64];
    int length;

    if(path && newpath)
    {
        f1 = fopen(path, "r");
        if(f1)
        {
            f2 = fopen(path, "w");
            if(f2)
            {
                length = 1;
                while(length > 0)
                {
                    length = fread(buffer, 1, sizeof(buffer), f1);
                    fwrite(buffer, 1, length, f2);
                }
                fclose(f2);
            }
            else
                send(fdes, ERROR_OPENING_DEST_FILE, sizeof(ERROR_OPENING_DEST_FILE)-1, 0);
            fclose(f1);
        }
        else
            send(fdes, ERROR_OPENING_SOURCE_FILE, sizeof(ERROR_OPENING_SOURCE_FILE)-1, 0);
    }
    else
        send(fdes, ARGUMENT_NOT_SPECIFIED, sizeof(ARGUMENT_NOT_SPECIFIED)-1, 0);

    return SHELL_CMD_EXIT;
}

shell_cmd_t sh_ls_cmd = {
    .name = "ls",
    .usage =
"prints directory content, relative to the current directory" SHELL_NEWLINE \
"flags:" SHELL_NEWLINE \
"\t-l  print details" SHELL_NEWLINE \
"ls [-l] [relpath]",
    .cmdfunc = sh_ls
};

shell_cmd_t sh_cd_cmd = {
    .name = "cd",
    .usage = "changes the current working directory",
    .cmdfunc = sh_cd
};

shell_cmd_t sh_rm_cmd = {
    .name = "rm",
    .usage = "removes the specified file(s)" SHELL_NEWLINE \
"rm file [file file ...]",
    .cmdfunc = sh_rm
};

shell_cmd_t sh_mkdir_cmd = {
    .name = "mkdir",
    .usage = "creates the specified directory",
    .cmdfunc = sh_mkdir
};

shell_cmd_t sh_echo_cmd = {
    .name = "echo",
    .usage = "add text to new file:" SHELL_NEWLINE \
"\techo 123 > file.txt" SHELL_NEWLINE \
"append text on new line in a file:" SHELL_NEWLINE \
"\techo abc >> file.txt" SHELL_NEWLINE \
"accepts `, ' and \" quotes" SHELL_NEWLINE \
"to preserve quotes:" SHELL_NEWLINE \
"\techo `\"key\": \"value\"` > file.txt",
    .cmdfunc = sh_echo
};

shell_cmd_t sh_cat_cmd = {
        .name = "cat",
        .usage = "reads the entire content of a file to the screen",
        .cmdfunc = sh_cat
};

shell_cmd_t sh_mv_cmd = {
		.name = "mv",
		.usage = "moves/renames a file" SHELL_NEWLINE \
"mv oldname newname",
		.cmdfunc = sh_mv
};

shell_cmd_t sh_cp_cmd = {
        .name = "cp",
        .usage = "copies a file from one location to another" SHELL_NEWLINE \
"cp file newfile",
        .cmdfunc = sh_cp
};
