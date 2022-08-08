# Git Config

```shell
$ git config --global branch.autosetuprebase always
$ git config --global color.branch true
$ git config --global color.diff true
$ git config --global color.interactive true
$ git config --global color.status true
$ git config --global color.ui true
$ git config --global commit.gpgsign true
$ git config --global core.autocrlf input
$ git config --global core.editor "code --wait"
$ git config --global diff.tool code
$ git config --global difftool.code.cmd "code --diff \$LOCAL \$REMOTE --wait"
$ git config --global gpg.program gpg
$ git config --global init.defaultbranch main
$ git config --global log.date relative
$ git config --global merge.tool code
$ git config --global mergetool.code.cmd "code --wait \$MERGED"
$ git config --global pull.default current
$ git config --global pull.rebase true
$ git config --global push.default current
$ git config --global rebase.autostash true
$ git config --global rerere.enabled true
$ git config --global stash.showpatch true
$ git config --global tag.gpgsign true
```

Alternative way which apply githooks folder

```shell
$ make git
```
