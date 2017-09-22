all: GIT

GIT: GIT2
	git push

GIT2: GIT3
	git commit -m "Teste Git"

GIT3:
	git add -A
