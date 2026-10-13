import type { MetadataRoute } from 'next';
import { plugins } from '@/lib/plugins';
import { SITE_URL } from '@/lib/site';

export const dynamic = 'force-static';

// Built from the catalog, so every plugin page is listed and nothing that does not exist is
export default function sitemap(): MetadataRoute.Sitemap {
  const pages = ['/', '/docs/', '/roadmap/', ...plugins.map((plugin) => `/plugins/${plugin.id}/`)];
  return pages.map((path) => ({ url: `${SITE_URL}${path}` }));
}
