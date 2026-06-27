import { Card } from '../components/ui';
export function SimplePage({ title, body }: { title: string; body: string }) { return <Card><h1 className="text-4xl font-black">{title}</h1><p className="mt-4 max-w-3xl text-lg opacity-80">{body}</p></Card>; }
