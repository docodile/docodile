import { test, expect } from "@playwright/test";

const pages = [
  "",
  "about",
  "blank",
  "contact",
  "colophon",
  "defaults",
  "follow",
  "links",
  "slashes",
  "blog",
  "blog/20260106",
];

pages.forEach((p) => {
  test(`page snapshot /${p}`, async ({ page }) => {
    await page.goto("http://localhost:6006/" + p);

    await expect(page).toHaveScreenshot({ fullPage: true });
  });
});
