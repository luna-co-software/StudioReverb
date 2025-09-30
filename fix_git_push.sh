#!/bin/bash

# Script to fix git push issues with remote repository

echo "Fixing git push issues..."
echo ""

# Check current status
echo "Current git status:"
git status --short
echo ""

# Fetch remote changes
echo "Fetching from remote..."
git fetch origin
echo ""

# Check what's on the remote
echo "Checking remote main branch..."
git ls-remote origin main
echo ""

# Try to rebase
echo "Attempting to rebase..."
if git pull --rebase origin main; then
    echo "✓ Rebase successful!"
    echo ""
    echo "Pushing to remote..."
    git push -u origin main
    echo "✓ Push successful!"
else
    echo "⚠ Rebase failed. Trying merge approach..."

    # Abort the rebase if it's in progress
    git rebase --abort 2>/dev/null || true

    # Try merge with unrelated histories
    echo "Attempting merge with --allow-unrelated-histories..."
    if git pull origin main --allow-unrelated-histories; then
        echo "✓ Merge successful!"

        # If there are conflicts, show them
        if [ -n "$(git diff --name-only --diff-filter=U)" ]; then
            echo ""
            echo "⚠ Conflicts detected in:"
            git diff --name-only --diff-filter=U
            echo ""
            echo "Please resolve conflicts manually, then run:"
            echo "  git add ."
            echo "  git commit"
            echo "  git push -u origin main"
        else
            echo "Pushing to remote..."
            git push -u origin main
            echo "✓ Push successful!"
        fi
    else
        echo ""
        echo "⚠ Both rebase and merge failed."
        echo ""
        echo "Option 1: Force push (will overwrite remote):"
        echo "  git push -u origin main --force"
        echo ""
        echo "Option 2: Clone the remote repo and copy your files:"
        echo "  cd .."
        echo "  git clone https://github.com/luna-co-software/StudioReverb.git StudioReverb-remote"
        echo "  cp -r StudioReverb/* StudioReverb-remote/"
        echo "  cd StudioReverb-remote"
        echo "  git add ."
        echo "  git commit -m 'Add Studio Reverb implementation'"
        echo "  git push"
    fi
fi

echo ""
echo "Done!"