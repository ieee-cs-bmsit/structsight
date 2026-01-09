# Publishing StructSight to VS Code Marketplace

This guide walks you through publishing the StructSight extension to the Visual Studio Code Marketplace under the **ieee-cs-bmsit** publisher name.

## Prerequisites

### 1. Create a VS Code Marketplace Publisher

**First-time setup:**

1. **Create a Microsoft Account** (if you don't have one)
   - Go to https://signup.live.com/
   - Or use existing Microsoft/Azure account

2. **Create an Azure DevOps Organization**
   - Visit https://dev.azure.com/
   - Sign in with your Microsoft account
   - Click "Create new organization"
   - Name it: `ieee-cs-bmsit` (or similar)

3. **Create a Personal Access Token (PAT)**
   - In Azure DevOps, click your profile → "Personal access tokens"
   - Click "+ New Token"
   - Name: `vsce-structsight`
   - Organization: Select your organization
   - Expiration: Custom defined (1-2 years recommended)
   - Scopes: Select "Marketplace" → **"Manage"**
   - Click "Create"
   - **IMPORTANT:** Copy the token immediately (you won't see it again)

4. **Create Publisher in VS Code Marketplace**
   - Go to https://marketplace.visualstudio.com/manage
   - Sign in with your Microsoft account
   - Click "Create publisher"
   - **Publisher ID**: `ieee-cs-bmsit`
   - **Display Name**: `IEEE Computer Society BMSIT&M`
   - **Description**: IEEE Computer Society chapter at BMS Institute of Technology & Management
   - Upload a logo (optional but recommended)
   - Click "Create"

### 2. Install vsce (VS Code Extension Manager)

```bash
npm install -g @vscode/vsce
```

### 3. Login with vsce

```bash
vsce login ieee-cs-bmsit
```

When prompted, paste your Personal Access Token.

---

## Pre-Publishing Checklist

Before publishing, ensure:

- [ ] All code is tested and working
- [ ] Native module builds successfully on target platforms
- [ ] README.md is complete with screenshots
- [ ] CHANGELOG.md is updated
- [ ] Version number is correct in `package.json`
- [ ] Publisher is set to `ieee-cs-bmsit` in `package.json`
- [ ] Icon file exists at `extension/resources/icon.png`
- [ ] All repository URLs point to `github.com/ieee-cs-bmsit/structsight`
- [ ] License file is included

---

## Creating Extension Icon

Create a **128x128** PNG icon:

1. Design a simple, recognizable icon
2. Save as `extension/resources/icon.png`
3. Recommended: Blue/tech-themed colors to match IEEE branding

**Quick icon creation:**
```bash
# Create resources directory
mkdir -p extension/resources

# Icon should represent memory/structure visualization
# You can use any graphic design tool or online icon maker
```

---

## Pre-Build Native Binaries (Important!)

Since the extension uses a native C++ addon, you should pre-build binaries for common platforms:

### Option 1: Build on each platform

**Windows (x64):**
```powershell
cd native
npm run build
# Output: build/Release/structsight_native.node
# Rename to: prebuilds/win32-x64/structsight_native.node
```

**Linux (x64):**
```bash
cd native
npm run build
# Output: build/Release/structsight_native.node
# Rename to: prebuilds/linux-x64/structsight_native.node
```

**macOS (arm64 and x64):**
```bash
cd native
npm run build
# Repeat for both architectures
```

### Option 2: Use GitHub Actions (Recommended)

Add workflow to build for all platforms automatically. The BUILD.md file includes a sample CI/CD configuration.

---

## Building the Extension Package

### 1. Ensure package.json is correct

Verify in `extension/package.json`:
```json
{
  "name": "structsight",
  "publisher": "ieee-cs-bmsit",
  "version": "0.1.0",
  "author": {
    "name": "Atul K M",
    "email": "ieee.cs.bmsit@gmail.com"
  },
  "repository": {
    "type": "git",
    "url": "https://github.com/ieee-cs-bmsit/structsight.git"
  }
}
```

### 2. Build the extension

```bash
# From project root
npm run build

# Or step by step:
cd native
npm run build

cd ../extension
npm run compile
```

### 3. Package the extension

```bash
cd extension
vsce package
```

This creates: `structsight-0.1.0.vsix`

**Troubleshooting packaging:**
- If vsce complains about missing files, check `.vscodeignore`
- Ensure all dependencies are in `dependencies`, not `devDependencies`
- Native module should be bundled in the package

---

## Publishing to Marketplace

### First-time publish

```bash
cd extension
vsce publish
```

This will:
1. Package the extension
2. Upload to VS Code Marketplace
3. Make it available as: `ieee-cs-bmsit.structsight`

### Publishing updates

When releasing new versions:

```bash
# Update version in package.json
# Option 1: Manual version bump
npm version patch  # 0.1.0 -> 0.1.1
npm version minor  # 0.1.0 -> 0.2.0
npm version major  # 0.1.0 -> 1.0.0

# Option 2: Let vsce handle it
vsce publish patch  # Bumps patch version and publishes
vsce publish minor  # Bumps minor version and publishes
vsce publish major  # Bumps major version and publishes
```

---

## Post-Publishing

### 1. Verify Installation

After publishing (may take 5-10 minutes to be available):

```bash
# Install from marketplace
code --install-extension ieee-cs-bmsit.structsight

# Or search in VS Code:
# Extensions → Search "StructSight"
```

### 2. Update GitHub Repository

```bash
git tag v0.1.0
git push origin v0.1.0

# Create GitHub release
# Go to: https://github.com/ieee-cs-bmsit/structsight/releases
# Click "Create a new release"
# Tag: v0.1.0
# Title: StructSight v0.1.0
# Attach the .vsix file
```

### 3. Monitor Marketplace

- Visit: https://marketplace.visualstudio.com/items?itemName=ieee-cs-bmsit.structsight
- Check for user reviews and ratings
- Monitor install count
- Respond to Q&A section

---

## Marketplace Listing Optimization

### Professional README for Marketplace

The marketplace uses your README.md. Make sure it has:

- ✅ Clear feature list
- ✅ Screenshots/GIFs showing the extension in action
- ✅ Installation instructions
- ✅ Usage examples
- ✅ Configuration options
- ✅ Troubleshooting section

### Add Screenshots

Create compelling screenshots showing:

1. **Hover tooltip** with struct info
2. **Webview visualization** with memory layout
3. **Optimization suggestions** panel
4. **Before/after** optimization comparison

Save in `extension/resources/` and reference in README:
```markdown
![Hover Analysis](resources/hover-demo.png)
![Memory Layout](resources/layout-demo.png)
```

### Extension Badge

Add a badge to README showing marketplace info:
```markdown
[![VS Code Marketplace](https://img.shields.io/visual-studio-marketplace/v/ieee-cs-bmsit.structsight)](https://marketplace.visualstudio.com/items?itemName=ieee-cs-bmsit.structsight)
[![Installs](https://img.shields.io/visual-studio-marketplace/i/ieee-cs-bmsit.structsight)](https://marketplace.visualstudio.com/items?itemName=ieee-cs-bmsit.structsight)
[![Rating](https://img.shields.io/visual-studio-marketplace/r/ieee-cs-bmsit.structsight)](https://marketplace.visualstudio.com/items?itemName=ieee-cs-bmsit.structsight)
```

---

## Known Issues & Solutions

### Issue: Native module not loading

**Solution:** Ensure native binaries are included:
```json
// In package.json, ensure files are not ignored
"files": [
  "out/**/*",
  "../native/build/Release/*.node"
]
```

### Issue: Large package size

**Solution:** Exclude unnecessary files in `.vscodeignore`:
```
src/**
node_modules/**
.vscode/**
*.md
!README.md
```

### Issue: Different LLVM versions on user machines

**Solution:** 
- Pre-build binaries with statically linked LLVM (if possible)
- Or document LLVM installation requirement clearly
- Provide fallback error messages with installation instructions

---

## Unpublishing (if needed)

To unpublish a version:
```bash
vsce unpublish ieee-cs-bmsit.structsight@0.1.0
```

To unpublish entire extension:
```bash
vsce unpublish ieee-cs-bmsit.structsight
```

⚠️ **Warning:** Unpublishing affects users who have already installed the extension.

---

## Alternative: Open VSX Registry

For users not using official VS Code (e.g., VSCodium), also publish to Open VSX:

```bash
npm install -g ovsx

# Login
ovsx login

# Publish
ovsx publish structsight-0.1.0.vsix -p <token>
```

---

## CI/CD Publishing

Automate publishing with GitHub Actions:

```yaml
name: Publish Extension

on:
  release:
    types: [created]

jobs:
  publish:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-node@v3
        with:
          node-version: 18
      
      - name: Install dependencies
        run: npm run install:all
      
      - name: Build
        run: npm run build
      
      - name: Publish to Marketplace
        env:
          VSCE_PAT: ${{ secrets.VSCE_PAT }}
        run: |
          cd extension
          npm install -g @vscode/vsce
          vsce publish -p $VSCE_PAT
```

Store your PAT in GitHub Secrets as `VSCE_PAT`.

---

## Summary Checklist

Before first publish:
- [ ] Create Azure DevOps organization
- [ ] Generate Personal Access Token
- [ ] Create `ieee-cs-bmsit` publisher
- [ ] Login with `vsce login ieee-cs-bmsit`
- [ ] Create extension icon (128x128 PNG)
- [ ] Add screenshots to README
- [ ] Build native module for target platforms
- [ ] Test the packaged extension locally
- [ ] Run `vsce package` to create .vsix
- [ ] Run `vsce publish` to publish
- [ ] Verify on marketplace
- [ ] Create GitHub release

---

## Support

For issues with publishing:
- **VS Code Marketplace:** https://aka.ms/vscode-publish
- **vsce tool:** https://github.com/microsoft/vscode-vsce
- **Publisher management:** https://marketplace.visualstudio.com/manage

For extension-specific help:
- Open issue: https://github.com/ieee-cs-bmsit/structsight/issues
- Contact: ieee.cs.bmsit@gmail.com

---

**Good luck with your first publish! 🚀**

*Remember: The extension name on the marketplace will be: `ieee-cs-bmsit.structsight`*
*Users will install it with: `code --install-extension ieee-cs-bmsit.structsight`*
