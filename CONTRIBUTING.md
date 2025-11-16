# Contributing to VibeBrowser

Hey there! Thanks for your interest in VibeBrowser. Whether you're fixing bugs, adding features, improving docs, or just poking around the code, we're glad you're here.

VibeBrowser is a monorepo with two main parts:
- **Agent** - The Chrome extension with AI features (TypeScript/React)
- **Browser** - The custom Chromium build (C++/Python)

Most folks start with the agent since it's way easier to set up and iterate on.

## Pick Your Path

<table>
<tr>
<td width="50%">

### 🤖 Agent Development

**What you'll work on:**
- AI agent features & tools
- UI/UX improvements
- Browser automation
- Testing & docs

**What you need:**
- Node.js 18+
- ~500MB disk space
- 10 minutes to set up

**Skills:** TypeScript, React, Chrome APIs

**[→ Agent Setup](#agent-development)**

</td>
<td width="50%">

### 🌐 Browser Development

**What you'll work on:**
- Chromium patches
- Build system
- Platform features
- Core browser stuff

**What you need:**
- ~100GB disk space
- 16GB+ RAM (recommended)
- 3+ hours for first build

**Skills:** C++, Python, Chromium internals

**[→ Browser Setup](#browser-development)**

</td>
</tr>
</table>

## Agent Development

The agent is a Chrome extension that provides AI-powered automation. Most contributors work here.

### Quick Setup

```bash
# 1. Navigate to agent directory
cd packages/vibebrowser-agent

# 2. Install dependencies
yarn install

# 3. Set up environment
cp .env.example .env
# Edit .env and add your LITELLM_API_KEY

# 4. Build the extension
yarn build:dev       # One-time build
```

### Load in VibeBrowser 

1. Open `chrome://extensions/`
2. Enable **Developer mode** (top right toggle)
3. Click **Load unpacked**
4. Select `packages/vibebrowser-agent/dist/`
5. Press Agent icon from extensions toolbar to open the agent panel

**For detailed setup, architecture, and code standards, see [Agent Contributing Guide](packages/vibebrowser-agent/CONTRIBUTING.md).**

## Browser Development

Building the custom Chromium browser requires significant disk space and time. Only go down this path if you're working on browser-level features like patches to Chromium itself.

### Prerequisites

- **~100GB disk space** for Chromium source
- **16GB+ RAM** (recommended)
- **Platform tools:**
  - macOS: Xcode + Command Line Tools
  - Linux: build-essential and dependencies
  - Windows: Visual Studio Build Tools

### Quick Setup

**1. Checkout Chromium source**

First, follow the official Chromium guide for your platform:
- **[Chromium: Get the Code](https://www.chromium.org/developers/how-tos/get-the-code/)**

This will set up `depot_tools` and fetch the ~100GB Chromium source tree. This typically takes 2-3 hours depending on your internet speed.

**2. Build VibeBrowser**

Once you have Chromium checked out, navigate to our build system:

```bash
cd packages/vibebrowser

# Debug build (for development)
# macOS
python build/build.py --config build/config/debug.macos.yaml --chromium-src /path/to/chromium/src --build

# Linux
python build/build.py --config build/config/debug.linux.yaml --chromium-src /path/to/chromium/src --build

# Windows
python build/build.py --config build/config/debug.windows.yaml --chromium-src /path/to/chromium/src --build

# Release build (for production)
# macOS
python build/build.py --config build/config/release.macos.yaml --chromium-src /path/to/chromium/src --build

# Linux
python build/build.py --config build/config/release.linux.yaml --chromium-src /path/to/chromium/src --build

# Windows
python build/build.py --config build/config/release.windows.yaml --chromium-src /path/to/chromium/src --build
```

The build typically takes 1-3 hours on modern hardware (M4 Max, Ryzen 9, etc.).

**For detailed instructions, see [Browser Build Guide](docs/BUILD.md).**

## Making Your First Contribution

Open a PR on GitHub with:
- **Clear title** in conventional commit format
- **Description** explaining what changed and why
- **Screenshots/videos** for UI changes
- **Link to related issues** (e.g., "Fixes #123")

### Sign the CLA

On your first PR, our bot will ask you to sign the Contributor License Agreement:

1. Read the [CLA document](CLA.md)
2. Comment on your PR: `I have read the CLA Document and I hereby sign the CLA`
3. The bot will record your signature (one-time thing)

## Code Standards

### TypeScript (Agent)

- **Strict typing** - Always declare types, avoid `any`
- **Zod schemas** - Use Zod instead of TypeScript interfaces
- **Path aliases** - Use `@/lib` not relative paths like `../`
- **Naming:**
  - Classes: `PascalCase`
  - Functions/variables: `camelCase`
  - Constants: `UPPERCASE`
  - Private methods: prefix with `_`

Example:
```typescript
import { z } from 'zod'

// Good: Zod schema with inline comments
export const ToolInputSchema = z.object({
  action: z.enum(['click', 'type']),  // Action to perform
  target: z.string().min(1),  // Element selector
  timeout: z.number().default(5000)  // Timeout in ms
})

export type ToolInput = z.infer<typeof ToolInputSchema>
```

### React (Agent UI)

- **Styling:** Tailwind CSS only (no SCSS or CSS modules)
- **Hooks:** Only at top level
- **Props:** Define with Zod schemas
- **Testing:** Vitest (not Jest)

### General

- Keep functions short (<20 lines ideally)
- Write tests for new features
- Use descriptive variable names
- Handle errors gracefully

**For detailed standards:**
- Agent: [packages/vibebrowser-agent/CLAUDE.md](packages/vibebrowser-agent/CLAUDE.md)
- Browser: Follow Chromium style guide

## Project Structure

```
monorepo/
├── packages/
│   ├── vibebrowser/              # Chromium build system
│   │   ├── build/             # Python build scripts
│   │   ├── chromium_patches/  # Patches to Chromium source
│   │   └── resources/         # Icons, configs
│   │
│   └── vibebrowser-agent/        # Chrome extension
│       ├── src/
│       │   ├── lib/           # Core agent logic
│       │   ├── sidepanel/     # Side panel UI
│       │   ├── newtab/        # New tab page
│       │   └── background/    # Extension background
│       └── docs/              # Architecture docs
│
├── docs/                       # General documentation
└── CONTRIBUTING.md            # This file
```

## Ways to Contribute

You don't need to write code to help out! Here are other ways:

### 🐛 Report Bugs

Found a bug? [Open an issue](https://github.com/VibeBrowser-ai/VibeBrowser/issues/new) with:
- Clear description
- Steps to reproduce
- Expected vs actual behavior
- Screenshots/videos
- Environment details (OS, browser version, VibeBrowser version)

### 💡 Suggest Features

Have an idea? [Share it here](https://github.com/VibeBrowser-ai/VibeBrowser/issues/99) or chat with us on [Discord](https://discord.gg/YKwjt5vuKr).

### 📚 Improve Documentation

- Write blog posts or guides

### 🧪 Test & Provide Feedback

- Try new features
- Test on different platforms
- Report edge cases
- Share your use cases

### 🎨 Design & UX

- Suggest UI improvements
- Create mockups
- Improve accessibility
- Enhance user experience
