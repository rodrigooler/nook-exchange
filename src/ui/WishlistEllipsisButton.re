module Styles = {
  open Css;
  let recipe =
    style([marginTop(px(6)), textAlign(center), fontSize(px(12))]);
  [@bs.module "../assets/dotdotdot.png"]
  external dotdotdotIcon: string = "default";
  let ellipsisButton =
    style([
      width(px(19)),
      height(px(19)),
      backgroundColor(Colors.white),
      backgroundImage(url(dotdotdotIcon)),
      backgroundSize(cover),
      borderWidth(zero),
      cursor(pointer),
      padding(zero),
      outlineStyle(none),
      opacity(0.5),
      transition(~duration=200, "all"),
      hover([important(opacity(1.))]),
    ]);
  let ellipsisButtonOpen = style([opacity(1.)]);
  let menu =
    style([
      backgroundColor(Colors.white),
      borderRadius(px(8)),
      padding2(~v=px(6), ~h=zero),
      overflow(hidden),
      Colors.darkLayerShadow,
    ]);
  let menuItem =
    style([
      backgroundColor(Colors.white),
      borderWidth(zero),
      display(block),
      width(pct(100.)),
      textAlign(`left),
      cursor(pointer),
      padding2(~v=px(4), ~h=px(12)),
      hover([backgroundColor(Colors.faintGray)]),
    ]);
  let menuItemRemove = style([color(Colors.red)]);
};

[@react.component]
let make = (~item: Item.t, ~variation, ~className) => {
  let me = UserStore.useMe()->Belt.Option.getExn;
  let ellipsisButtonRef = React.useRef(Js.Nullable.null);
  let (showEllipsisMenu, setShowEllipsisMenu) = React.useState(() => false);

  <>
    <button
      className={Cn.make([
        Styles.ellipsisButton,
        Cn.ifTrue(Styles.ellipsisButtonOpen, showEllipsisMenu),
        className,
      ])}
      onClick={_ => setShowEllipsisMenu(_ => true)}
      ref={ReactDOMRe.Ref.domRef(ellipsisButtonRef)}
    />
    {showEllipsisMenu
       ? <ReactAtmosphere.PopperLayer
           reference=ellipsisButtonRef
           onOutsideClick={() => setShowEllipsisMenu(_ => false)}
           options={
             placement: Some("bottom"),
             modifiers:
               Some([|
                 {
                   "name": "offset",
                   "options": {
                     "offset": [|0, 4|],
                   },
                 },
               |]),
           }
           render={_renderArgs =>
             <div className=Styles.menu>
               <button
                 onClick={_ => {
                   UserStore.setItemStatus(
                     ~itemId=item.id,
                     ~variation,
                     ~status=ForTrade,
                   )
                 }}
                 className=Styles.menuItem>
                 {React.string("Move to For Trade")}
               </button>
               <button
                 onClick={_ => {
                   UserStore.setItemStatus(
                     ~itemId=item.id,
                     ~variation,
                     ~status=CanCraft,
                   )
                 }}
                 className=Styles.menuItem>
                 {React.string("Move to Can Craft")}
               </button>
               {me.enableCatalogCheckbox
                  ? <button
                      onClick={_ => {
                        UserStore.setItemStatus(
                          ~itemId=item.id,
                          ~variation,
                          ~status=CatalogOnly,
                        )
                      }}
                      className=Styles.menuItem>
                      {React.string("Move to Catalog")}
                    </button>
                  : React.null}
               <button
                 onClick={_ => {
                   UserStore.removeItem(~itemId=item.id, ~variation)
                 }}
                 className={Cn.make([Styles.menuItem, Styles.menuItemRemove])}>
                 {React.string("Remove from Wishlist")}
               </button>
             </div>
           }
         />
       : React.null}
  </>;
};