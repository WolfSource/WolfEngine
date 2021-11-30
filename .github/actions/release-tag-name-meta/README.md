# Release tag meta action

Extract release metadata from the tag.

## Inputs

## `git-ref`

**Required** The git ref.

## Outputs

## `name`

The prerelease/release name.

## `prerelease`

Is it release candidate?

## `tag`

The tag version (without the refs/tags/ prefix).

## `version`

The prerelease/release version (without the refs/tags/v prefix).

## Example usage

uses: actions/release-tag-meta@v1
with:
  git-ref: 'refs/tags/v0.1.0'
