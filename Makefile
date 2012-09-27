smb: smb.c
	gcc smb.c -o SMB

stat: stat.c
	gcc stat.c -o STAT

user: user.c
	gcc user.c -o user

all: smb stat user

clean:
	rm SMB STAT user
