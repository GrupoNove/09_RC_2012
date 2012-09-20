smb: smb.c
	gcc &^ -o SMB

stat: stat.c
	gcc &^ -o STAT

user: user.c
	gcc &^ -o user