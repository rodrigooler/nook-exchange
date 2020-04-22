module Styles = {
  open Css;
  let root =
    style([
      width(px(368)),
      margin3(~top=px(32), ~bottom=zero, ~h=auto),
      media("(min-width: 660px)", [width(px(560))]),
      media("(min-width: 860px)", [width(px(752))]),
      media("(min-width: 1040px)", [width(px(944))]),
      media("(min-width: 1240px)", [width(px(1136))]),
      media("(min-width: 1440px)", [width(px(1328))]),
      padding(px(32)),
      borderRadius(px(16)),
      position(relative),
      media(
        "(max-width: 470px)",
        [width(auto), padding(px(16)), borderRadius(zero)],
      ),
    ]);
  let anchor = style([position(absolute), top(px(-100))]);
  let rootForTrade = style([backgroundColor(hex("8FCDE0a0"))]);
  let rootCanCraft = style([backgroundColor(hex("f1e26fa0"))]);
  let rootMini = style([backgroundColor(hex("fffffff0"))]);
  let sectionTitle = style([fontSize(px(24)), marginBottom(px(16))]);
  let filterBar = style([marginBottom(zero)]);
  let cards =
    style([
      paddingTop(px(16)),
      marginRight(px(-16)),
      media("(max-width: 470px)", [paddingTop(zero)]),
    ]);
  let cardsMini =
    style([
      justifyContent(flexStart),
      paddingTop(px(32)),
      marginLeft(px(-8)),
      marginRight(px(-8)),
    ]);
  let metaIcons = style([opacity(0.), transition(~duration=200, "all")]);
  let card =
    style([
      backgroundColor(hex("fffffff0")),
      borderRadius(px(8)),
      display(flexBox),
      flexDirection(column),
      alignItems(center),
      marginRight(px(16)),
      marginBottom(px(16)),
      padding3(~top=px(24), ~bottom=px(8), ~h=px(8)),
      position(relative),
      width(px(160)),
      transition(~duration=200, "all"),
      media("(max-width: 430px)", [width(pct(100.))]),
      hover([selector("& ." ++ metaIcons, [opacity(1.)])]),
    ]);
  let mainImageWrapperWithRecipe = style([marginBottom(px(16))]);
  let name =
    style([fontSize(px(16)), marginBottom(px(4)), textAlign(center)]);
  let userNote =
    style([
      borderTop(px(1), solid, hex("f0f0f0")),
      unsafe("alignSelf", "stretch"),
      lineHeight(px(18)),
      marginTop(px(-8)),
      padding3(~top=px(8), ~bottom=zero, ~h=px(4)),
    ]);
  let removeButton = style([top(px(9)), bottom(initial)]);
  let cardMini = style([position(relative)]);
  let cardMiniImage =
    style([display(block), width(px(64)), height(px(64))]);
  let cardMiniRecipe =
    style([
      display(block),
      width(px(32)),
      height(px(32)),
      position(absolute),
      bottom(px(-4)),
      right(px(-4)),
    ]);
  let sectionToggles =
    style([
      position(absolute),
      right(px(32)),
      top(px(36)),
      display(flexBox),
      flexDirection(column),
      alignItems(flexEnd),
      media(
        "(max-width: 600px)",
        [position(static), textAlign(center), flexDirection(row)],
      ),
      media("(max-width: 470px)", [marginBottom(px(16))]),
    ]);
  let showRecipesBox = style([marginLeft(px(16))]);
  let showRecipesLabel = style([fontSize(px(16)), marginRight(px(8))]);
  let showRecipesCheckbox =
    style([
      fontSize(px(24)),
      margin(zero),
      position(relative),
      top(px(-2)),
    ]);
  let recipe =
    style([marginTop(px(6)), textAlign(center), fontSize(px(12))]);
};

open Belt;

module UserItemCard = {
  [@react.component]
  let make =
      (~itemId, ~variation, ~userItem: User.item, ~editable, ~showRecipe) => {
    let item = Item.getItem(~itemId);
    <div className={Cn.make([Styles.card])}>
      <div className=ItemCard.Styles.body>
        <div
          className={Cn.make([
            ItemCard.Styles.mainImageWrapper,
            Cn.ifTrue(Styles.mainImageWrapperWithRecipe, item.isRecipe),
          ])}>
          <img
            src={Item.getImageUrl(~item, ~variant=variation)}
            className=ItemCard.Styles.mainImage
          />
          {item.isRecipe
             ? <img
                 src={Constants.imageUrl ++ "/DIYRecipe.png"}
                 className=ItemCard.Styles.recipeIcon
               />
             : React.null}
        </div>
        <div className=Styles.name> {React.string(item.name)} </div>
        {switch (showRecipe, item.recipe) {
         | (true, Some(recipe)) =>
           <div className=Styles.recipe>
             {recipe
              ->Array.map(((itemId, quantity)) =>
                  <div key=itemId>
                    {React.string(itemId ++ " x " ++ string_of_int(quantity))}
                  </div>
                )
              ->React.array}
           </div>
         | _ => React.null
         }}
      </div>
      <div className={Cn.make([ItemCard.Styles.metaIcons, Styles.metaIcons])}>
        {switch (item.recipe) {
         | Some(recipe) => <ItemCard.RecipeIcon recipe />
         | None => React.null
         }}
        {if (item.orderable) {
           <ItemCard.OrderableIcon />;
         } else {
           React.null;
         }}
      </div>
      {editable
         ? <>
             <UserItemNote itemId={item.id} variation userItem />
             <button
               className={Cn.make([
                 ItemCard.Styles.removeButton,
                 Styles.removeButton,
               ])}
               title="Remove"
               onClick={_ => {
                 UserStore.removeItem(~itemId=item.id, ~variation)
               }}>
               {React.string({j|❌|j})}
             </button>
           </>
         : (
           if (userItem.note->Js.String.length > 0) {
             <div className=Styles.userNote>
               {Emoji.parseText(userItem.note)}
             </div>;
           } else {
             React.null;
           }
         )}
    </div>;
  };
};

module UserItemCardMini = {
  [@react.component]
  let make = (~itemId, ~variation) => {
    let item = Item.getItem(~itemId);
    <div className=Styles.cardMini>
      <img
        src={Item.getImageUrl(~item, ~variant=variation)}
        title={item.name}
        className=Styles.cardMiniImage
      />
      {item.isRecipe
         ? <img
             src={Constants.imageUrl ++ "/DIYRecipe.png"}
             className=Styles.cardMiniRecipe
           />
         : React.null}
    </div>;
  };
};

module Section = {
  let numResultsPerPage = 14;

  let randomString = () => Js.Math.random()->Js.Float.toString;

  let getMaxResults = (~viewportWidth) =>
    if (viewportWidth >= 1440) {
      14;
    } else if (viewportWidth >= 1240) {
      12;
    } else if (viewportWidth >= 1040) {
      10;
    } else if (viewportWidth >= 860) {
      12;
    } else if (viewportWidth >= 660) {
      9;
    } else {
      8;
    };

  [@react.component]
  let make =
      (
        ~username: string,
        ~status: User.itemStatus,
        ~userItems: array(((string, int), User.item)),
        ~editable,
      ) => {
    let id = React.useMemo0(() => randomString());
    let viewportWidth = Utils.useViewportWidth();
    let maxResults = getMaxResults(~viewportWidth);
    let (showRecipes, setShowRecipes) = React.useState(() => false);
    let (showMini, setShowMini) = React.useState(() => false);
    let (filters, setFilters) =
      React.useState(() =>
        (
          {text: "", mask: None, category: None, sort: Category}: ItemFilters.t
        )
      );
    let (pageOffset, setPageOffset) = React.useState(() => 0);
    let filteredItems =
      React.useMemo2(
        () => {
          let sortFn = ItemFilters.getSort(~filters);
          userItems->Belt.Array.keep((((itemId, _), _)) =>
            ItemFilters.doesItemMatchFilters(
              ~item=Item.getItem(~itemId),
              ~filters,
            )
          )
          |> Js.Array.sortInPlaceWith((((aId, _), _), ((bId, _), _)) =>
               sortFn(Item.getItem(~itemId=aId), Item.getItem(~itemId=bId))
             );
        },
        (userItems, filters),
      );
    let numResults = filteredItems->Belt.Array.length;
    let showFilters = false; // userItems->Belt.Array.length > numResultsPerPage;

    let anchorId =
      switch (status) {
      | ForTrade => "for-trade"
      | CanCraft => "can-craft"
      | Wishlist => "wishlist"
      };
    let anchorRef = React.useRef(Js.Nullable.null);
    React.useEffect0(() => {
      let url = ReasonReactRouter.dangerouslyGetInitialUrl();
      if (url.hash == anchorId) {
        open Webapi.Dom;
        let anchorElement = Utils.getElementForDomRef(anchorRef);
        anchorElement->Element.scrollIntoView;
      };
      None;
    });

    <div
      className={Cn.make([
        Styles.root,
        Cn.ifTrue(Styles.rootForTrade, status == ForTrade),
        Cn.ifTrue(Styles.rootCanCraft, status == CanCraft),
        Cn.ifTrue(Styles.rootMini, showMini),
      ])}>
      <div
        className=Styles.anchor
        id=anchorId
        ref={ReactDOMRe.Ref.domRef(anchorRef)}
      />
      <div className=Styles.sectionTitle>
        {React.string(
           switch (status) {
           | Wishlist => {j|🙏 Wishlist|j}
           | ForTrade => {j|✅ For Trade|j}
           | CanCraft => {j|🔨 Can Craft|j}
           },
         )}
      </div>
      <div className=Styles.sectionToggles>
        {userItems->Array.length > 16
           ? <div>
               <label htmlFor=id className=Styles.showRecipesLabel>
                 {React.string("Miniature")}
               </label>
               <input
                 id
                 type_="checkbox"
                 checked=showMini
                 onChange={e => {
                   let checked = ReactEvent.Form.target(e)##checked;
                   Analytics.Amplitude.logEventWithProperties(
                     ~eventName="Miniature Mode Clicked",
                     ~eventProperties={"checked": checked, "status": status},
                   );
                   setShowMini(_ => checked);
                 }}
                 className=Styles.showRecipesCheckbox
               />
             </div>
           : React.null}
        {if (status == CanCraft && !showMini) {
           <div className=Styles.showRecipesBox>
             <label
               htmlFor="craftShowRecipe" className=Styles.showRecipesLabel>
               {React.string("Show Recipes")}
             </label>
             <input
               id="craftShowRecipe"
               type_="checkbox"
               checked=showRecipes
               onChange={e => {
                 let checked = ReactEvent.Form.target(e)##checked;
                 Analytics.Amplitude.logEventWithProperties(
                   ~eventName="Show Recipes Clicked",
                   ~eventProperties={"checked": checked},
                 );
                 setShowRecipes(_ => checked);
               }}
               className=Styles.showRecipesCheckbox
             />
           </div>;
         } else {
           React.null;
         }}
      </div>
      {showFilters
         ? <div
             className={Cn.make([
               ItemBrowser.Styles.filterBar,
               Styles.filterBar,
             ])}>
             <ItemFilters
               filters
               onChange={filters => {
                 setFilters(_ => filters);
                 setPageOffset(_ => 0);
               }}
               showCategorySort=true
             />
             {!showMini
                ? <ItemFilters.Pager
                    numResults
                    pageOffset
                    numResultsPerPage
                    setPageOffset
                  />
                : React.null}
           </div>
         : React.null}
      <div
        className={Cn.make([
          ItemBrowser.Styles.cards,
          Styles.cards,
          Cn.ifTrue(Styles.cardsMini, showMini),
        ])}>
        {filteredItems
         ->(
             showMini
               ? x => x
               : Belt.Array.slice(
                   ~offset=0,
                   ~len=numResults > maxResults ? maxResults - 1 : maxResults,
                 )
           )
         ->Belt.Array.mapU((. ((itemId, variation), userItem)) => {
             showMini
               ? <UserItemCardMini
                   itemId
                   variation
                   key={itemId ++ string_of_int(variation)}
                 />
               : <UserItemCard
                   itemId
                   variation
                   userItem
                   editable
                   showRecipe=showRecipes
                   key={itemId ++ string_of_int(variation)}
                 />
           })
         ->(
             numResults > maxResults
               ? Belt.Array.concat(
                   _,
                   [|
                     <Link
                       path={
                         "/u/"
                         ++ username
                         ++ "/"
                         ++ (
                           switch (status) {
                           | Wishlist => "wishlist"
                           | CanCraft => "can-craft"
                           | ForTrade => "for-trade"
                           }
                         )
                       }
                       className=Styles.card
                       key="link">
                       <div className=Styles.name>
                         {React.string("See all")}
                       </div>
                     </Link>,
                   |],
                 )
               : (x => x)
           )
         ->React.array}
      </div>
      {showFilters && !showMini
         ? <div className=ItemBrowser.Styles.bottomFilterBar>
             <ItemFilters.Pager
               numResults
               pageOffset
               numResultsPerPage
               setPageOffset
             />
           </div>
         : React.null}
    </div>;
  };
};

let getUrl =
    (
      ~url: ReasonReactRouter.url,
      ~urlSearchParams: Webapi.Url.URLSearchParams.t,
    ) => {
  "/"
  ++ Js.Array.joinWith("/", Belt.List.toArray(url.path))
  ++ (
    switch (Webapi.Url.URLSearchParams.toString(urlSearchParams)) {
    | "" => ""
    | search => "?" ++ search
    }
  );
};

// TODO
let numResultsPerPage = 20;
let getNumResultsPerPage = (~viewportWidth) =>
  if (viewportWidth >= 1440) {
    28;
  } else if (viewportWidth >= 1240) {
    24;
  } else if (viewportWidth >= 1040) {
    25;
  } else if (viewportWidth >= 860) {
    20;
  } else if (viewportWidth >= 660) {
    18;
  } else {
    12;
  };

[@react.component]
let make = (~user: User.t, ~list: string, ~url: ReasonReactRouter.url) => {
  let viewportWidth = Utils.useViewportWidth();
  let numResultsPerPage = getNumResultsPerPage(~viewportWidth);
  let status = User.urlToItemStatus(list);
  let userItems =
    React.useMemo2(
      () =>
        user.items
        ->Js.Dict.entries
        ->Belt.Array.keepMapU((. (itemKey, item: User.item)) =>
            item.status == status
              ? Some((User.fromItemKey(~key=itemKey), item)) : None
          ),
      (user, list),
    );

  let (showMini, setShowMini) = React.useState(() => false);
  let (filters, pageOffset) =
    React.useMemo1(
      () =>
        ItemFilters.fromUrlSearch(
          ~urlSearch=url.search,
          ~defaultSort=SellPriceDesc,
        ),
      [|url.search|],
    );
  let setFilters = (filters: ItemFilters.t) => {
    Analytics.Amplitude.logEventWithProperties(
      ~eventName="Filters Changed",
      ~eventProperties={
        "filterText": filters.text,
        "filterMask": filters.mask,
        "filterCategory": filters.category,
        "filterSort": filters.sort,
      },
    );
    let urlSearchParams =
      Webapi.Url.URLSearchParams.makeWithArray(
        ItemFilters.serialize(
          ~filters,
          ~defaultSort=SellPriceDesc,
          ~pageOffset=0,
        ),
      );
    ReasonReactRouter.push(getUrl(~url, ~urlSearchParams));
  };
  let setPageOffset = f => {
    let nextPageOffset = f(pageOffset);
    let urlSearchParams =
      Webapi.Url.URLSearchParams.makeWithArray(
        ItemFilters.serialize(
          ~filters,
          ~defaultSort=SellPriceDesc,
          ~pageOffset=nextPageOffset,
        ),
      );
    ReasonReactRouter.push(getUrl(~url, ~urlSearchParams));
  };
  let filteredItems =
    React.useMemo2(
      () => {
        let sortFn = ItemFilters.getSort(~filters);
        userItems->Belt.Array.keep((((itemId, _), _)) =>
          ItemFilters.doesItemMatchFilters(
            ~item=Item.getItem(~itemId),
            ~filters,
          )
        )
        |> Js.Array.sortInPlaceWith((((aId, _), _), ((bId, _), _)) =>
             sortFn(Item.getItem(~itemId=aId), Item.getItem(~itemId=bId))
           );
      },
      (userItems, filters),
    );
  let numResults = filteredItems->Belt.Array.length;

  let renderListLink = status => {
    <Link
      path={"/u/" ++ user.username ++ "/" ++ User.itemStatusToUrl(status)}>
      {React.string(User.itemStatusToString(status))}
    </Link>;
  };

  <div className=Styles.root>
    <div>
      {renderListLink(ForTrade)}
      {renderListLink(CanCraft)}
      {renderListLink(Wishlist)}
    </div>
    <ItemFilters.CategoryButtons
      filters
      onChange={filters => setFilters(filters)}
    />
    <div
      className={Cn.make([ItemBrowser.Styles.filterBar, Styles.filterBar])}>
      <ItemFilters
        filters
        onChange={filters => setFilters(filters)}
        showCategorySort=true
      />
      <ItemFilters.Pager
        numResults
        pageOffset
        numResultsPerPage
        setPageOffset
      />
    </div>
    <div
      className={Cn.make([
        ItemBrowser.Styles.cards,
        Styles.cards,
        Cn.ifTrue(Styles.cardsMini, showMini),
      ])}>
      {filteredItems
       ->(
           showMini
             ? x => x
             : Belt.Array.slice(
                 ~offset=pageOffset * numResultsPerPage,
                 ~len=numResultsPerPage,
               )
         )
       ->Belt.Array.mapU((. ((itemId, variation), userItem)) => {
           showMini
             ? <UserItemCardMini
                 itemId
                 variation
                 key={itemId ++ string_of_int(variation)}
               />
             : <UserItemCard
                 itemId
                 variation
                 userItem
                 editable=false
                 showRecipe=false
                 key={itemId ++ string_of_int(variation)}
               />
         })
       ->React.array}
    </div>
    {!showMini
       ? <div className=ItemBrowser.Styles.bottomFilterBar>
           <ItemFilters.Pager
             numResults
             pageOffset
             numResultsPerPage
             setPageOffset={f => {
               setPageOffset(f);
               Webapi.Dom.(window |> Window.scrollTo(0., 0.));
             }}
           />
         </div>
       : React.null}
  </div>;
};