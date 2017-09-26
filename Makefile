all: GIT

GIT: GIT2
	git push

GIT2: GIT3
	git commit -m "Controle Ganhos"

GIT3:
	git add -A
