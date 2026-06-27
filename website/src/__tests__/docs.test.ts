import { describe, expect, it } from 'vitest';
import { searchDocs } from '../data/docs';
describe('offline docs search', () => { it('finds parser docs', () => expect(searchDocs('parser').length).toBeGreaterThan(0)); });
