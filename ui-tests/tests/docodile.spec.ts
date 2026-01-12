import { test, expect } from "@playwright/test";

const pages = [
  "",
  "getting-started",
  "getting-started/contributing",
  "setup",
  "setup/appearance",
  "setup/navigation",
  "reference",
  "reference/markdown",
  "reference/markdown-extensions",
  "reference/markdown-extensions/admonitions",
  "reference/markdown-extensions/collapsible-headings",
  "reference/markdown-extensions/tables",
  "reference/organisation",
];

pages.forEach((p) => {
  test(`page snapshot /${p}`, async ({ page }) => {
    await page.goto("http://localhost:6006/" + p);

    await expect(page).toHaveScreenshot({ fullPage: true });
  });
});
