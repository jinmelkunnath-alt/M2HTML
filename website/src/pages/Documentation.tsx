import { useState } from 'react';
import { docs, searchDocs } from '../data/docs';
import { Card } from '../components/ui';
export function Documentation() { const [q,setQ]=useState(''); const rows=q?searchDocs(q):docs; return <div className="grid gap-6 lg:grid-cols-[280px_1fr]"><aside className="rounded-3xl bg-white/10 p-4"><input aria-label="Search documentation" value={q} onChange={e=>setQ(e.target.value)} className="w-full rounded-xl bg-black/20 p-3" />{docs.map(d=><a className="mt-2 block rounded-lg p-2 hover:bg-white/10" href={`#${d.slug}`} key={d.slug}>{d.title}</a>)}</aside><div className="space-y-4">{rows.map(d=><Card key={d.slug}><h2 id={d.slug} className="text-2xl font-bold">{d.title}</h2><p className="mt-3 opacity-80">{d.body}</p></Card>)}</div></div>; }
