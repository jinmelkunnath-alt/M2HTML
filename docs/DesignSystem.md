# DesignSystem

MarkForge Prompt 5 adds the official static website in the website/ directory. The site is a React, TypeScript, Vite and Tailwind application with Framer Motion transitions, Monaco-powered editing, offline documentation search and a local WebAssembly integration.

## Offline Guarantees

The website has no backend, no analytics, no authentication, no remote database and no runtime network dependency. After the static bundle is built, the playground loads the local wasm/markforge.wasm artifact and executes entirely in the browser.

## Architecture

- React pages live in website/src/pages.
- Reusable UI components live in website/src/components.
- Hooks live in website/src/hooks.
- Documentation and examples are indexed locally in website/src/data.
- WASM integration lives in website/src/wasm/markforgeWasm.ts and website/wasm/markforge_wasm.c.
- Static assets and examples live in website/public.

## Accessibility

Components use semantic elements, keyboard-focus styles, ARIA roles where appropriate, reduced-motion compatible transitions and responsive layouts.

The design system includes Button, Card, Tabs, Panel, Skeleton, Notification, Layout, command palette and performance dashboard components. Icons are local text/SVG assets and no icon CDN is required.
