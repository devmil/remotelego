import { LegoRemoteNg2Page } from './app.po';

describe('lego-remote-ng2 App', function() {
  let page: LegoRemoteNg2Page;

  beforeEach(() => {
    page = new LegoRemoteNg2Page();
  });

  it('should display message saying app works', () => {
    page.navigateTo();
    expect(page.getParagraphText()).toEqual('app works!');
  });
});
