smb: smb.c
	gcc smb.c -o SMB

stat: stat.c
	gcc stat.c -o STAT

user: user.c
	gcc user.c -o user

dummyUser: dummyUser.c
	gcc dummyUser.c -o dummyUser

dummySMB: dummySMB.c
	gcc dummySMB.c -o dummySMB

all: smb stat user dummyUser dummySMB

clean:
	rm SMB STAT user dummyUser dummySMB
